#include "LPC214x.h"

void init_uart()
{
PINSEL0|=0x00050000;
  
	U1LCR|=0x87; //setting DLAB=1 for UBRR conf 3rd bit=1 for parity enable(oddparity check by default)
	// 2nd bit=1 for 2 stop bits(bydeafult 1 stop bit) //0th and 1st bit =1 for 8 bit chara(5 bit by default)
	U1DLL|=0xB7; //U1DLL=183 for UBRR=9605
	U1FDR|=0x000000F1; //MULVAL=15 DIV val=1
	U1LCR&=0x7F; //setting DLAB=0 for data

}

void init_clock()
{
  PLL0CFG|=0x04; //setting CCLK= 60MHz from FOSC = 12MHZ and M=5 MSEL=0x04 and P=0
	VPBDIV|=0x02; //setting PCLK = 30MHz and CCLK= 60MHz 
}

void send_character(unsigned char val)
{
    while(!((U1LSR&(1<<5))==0x20));
		//checking THRE(check transmit hold ready empty)
		U1THR=val;
}


void send_str(unsigned char *value)
{
	 int index = 0;
  while(value[index] != '\0')
 	{
	  send_character(value[index++]);
	}
}


void delay()
{
for(int i=0;i<500;i++)
			{
				for(int j=0;j<1000;j++)
				{;
				}
			}
}