#include "uartStartup.h"
#include "stdio.h"


/*This code is written for the ADC 0 channel 3 (i.e AD0.3)
Hence, the ADDR3 A/D Channel 3 Data Register will be used. 
This register contains the result of the most recent conversion completed on
channel 3.

CLKDIV The APB clock (PCLK) is divided by (this value plus one) to produce the clock for the
A/D converter, which should be less than or equal to 4.5 MHz
*/

//----------- ADC Control Register Bit Mapping ------------------
#define ADC_CONTROL_REGISTER_SEL 0
#define ADC_CONTROL_REGISTER_CLKDIV 8 // for 3MHz    15/5 = 3, 5-1 = 4 will be written to the CLKDIV
#define ADC_CONTROL_REGISTER_BURST 16      // To Select the Burst Mode
#define ADC_CONTROL_REGISTER_CLKS 17       // for 000, we have 10 Bit resolution
#define ADC_CONTROL_REGISTER_PDN  21       // POWER DOWN MODE
#define ADC_CONTROL_REGISTER_START 24      // to start the conversion
//---------------------------------------------------------------

//-----------ADC Data Registers  ADDR0 to ADDR7 for AD0(AD0DR0 to AD0DR7) and for AD1(AD1DR0 to AD1DR7) -------------
/*The A/D Data Register hold the result when an A/D conversion is complete, and also
include the flags that indicate when a conversion has been completed and when a
conversion overrun has occurred.*/
#define ADC_DATA_REGISTER_RESULT 6 // when DONE bit is 1, this RESULT 15:6 will have 10-bit ADC val
#define ADC_DATA_REGISTER_DONE 31  // This bit is set to 1 when an A/D conversion completes. It is cleared when this register is read.
//----------------------------------------------------------------

//------------ ADC Selection Bit --------------------------------
// these macros provide the value for the given bit number is a register

#define ADC0_0 0
#define ADC0_1 2
#define ADC0_2 4
#define ADC0_3 8
#define ADC0_4 16
#define ADC0_5 32
#define ADC0_6 64
#define ADC0_7 128
//---------------------------------------------------------------

/*A/D Data Registers (ADDR0 to ADDR7, ADC0: AD0DR0 to AD0DR7 -
0xE003 4010 to 0xE003 402C and ADC1: AD1DR0 to AD1DR7-
0xE006 0010 to 0xE006 402C)
The A/D Data Register hold the result when an A/D conversion is complete, and also
include the flags that indicate when a conversion has been completed and when a
conversion overrun has occurred*/

//-------------- Conditional Macros -------------
#define BIT_STATUS(REGISTER,BIT) ((REGISTER &(1<<BIT)))
//-----------------------------------------------------------------

#define HIGH 1

#define SBIT(REGISTER,BIT,VALUE) (REGISTER |(VALUE<<BIT))
#define CLR(REGISTER,BIT,VALUE) (REGISTER &(~(VALUE<<BIT)))


/*----PINSEL1----------
29:28 P0.30 00 GPIO Port 0.30 0
						01 AD0.3 -------------- this function should be selected for ADC
						10 EINT3
						11 Capture 0.0 (Timer 0)
//-------------------*/

  

void init_adc(void);              // to perform basic initialization of our ADC
unsigned short get_ADC_val(void);  // it will start the conversion and then return the value to the caller
char value;
int main(void)
{	
	init_adc();
	init_clock();
	init_uart();
	
	//send_str((unsigned char *)"Welcome");				

	char value[2];
  	
	while(1)
		{		
	     
			sprintf(value, "%hd",get_ADC_val());  // converting the decimal value to the string  and storing the result in the array
      // this function will convert the ADC value (unsigned int) into string (char arr) 			
			
			// Displaying the data on UART/LCD
      send_str((unsigned char *)"Value: ");
			send_str((unsigned char *)value);
      		
    	send_character('\r');
			send_character('\n');
		}
		
	return 0;
}

void init_adc()
{
 PINSEL1 = SBIT(PINSEL1,28,1);  // to select the ADC funciton of the given pin
 
 //----------Initializing the ADC Control Register              
 AD0CR  |= SBIT(AD0CR,ADC_CONTROL_REGISTER_SEL,ADC0_3) |
           SBIT(AD0CR,ADC_CONTROL_REGISTER_CLKDIV,4)   |
           SBIT(AD0CR,ADC_CONTROL_REGISTER_BURST,HIGH) |
           SBIT(AD0CR,ADC_CONTROL_REGISTER_CLKS,0)     |
	         SBIT(AD0CR,ADC_CONTROL_REGISTER_PDN,HIGH)   | 
	         SBIT(AD0CR,ADC_CONTROL_REGISTER_START,1);       
	// now the ADC is not running because "burst [1]" requires "START" bits (B1B2B3) to be [000]
}

unsigned short get_ADC_val()
{
 AD0CR |=  SBIT(AD0CR,ADC_CONTROL_REGISTER_START,0);         // start ADC Conversion
	
 // once the conversion is started, we will now monitor the DONE bit in the AD0DR3 (ch3)
 while(BIT_STATUS(AD0DR3,ADC_DATA_REGISTER_DONE) != (1<<ADC_DATA_REGISTER_DONE));  
 // waiting for the DONE bit to become high 
  
 // Now since the conversion is done, we can read the converted value from the Bits starting from RESULT 	
 //unsigned short value = (AD0DR3 & (0xFFC0));  // "value" will now have the converted 10-bit value	
 
	// masking the unused bits and extracting the values from the bit 6 to bit 15, (10 bit) value and then
  // right shifting the	values to 6 places, in order to put the LSB at 0th position
	unsigned short value = ((AD0DR3 & (0xFFC0))>>6);  // "value" will now have the converted 10-bit value	
 
	
 return(value);    // returning the "value" to the caller
}
