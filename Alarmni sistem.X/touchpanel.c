#include<p30fxxxx.h>
#include <stdlib.h>
#include "driverGLCD.h"
#include "adc.h"
#include "funkcije.h"
#include "serijska.h"
#include "touchpanel.h"

int sirovi1,sirovi0,sirovi2;
int broj,broj1,broj2,temp0,temp1,temp2,temp; 

#define DRIVE_A PORTCbits.RC13
#define DRIVE_B PORTCbits.RC14

void ConfigureTSPins(void)
{
	//ADPCFGbits.PCFG10=1;
	//ADPCFGbits.PCFG7=digital;

	//TRISBbits.TRISB10=0;
	TRISCbits.TRISC13=0;
    TRISCbits.TRISC14=0;
	
	//LATCbits.LATC14=0;
	//LATCbits.LATC13=0;
}

void Delay(unsigned int N)
{
	unsigned int i;
	for(i=0;i<N;i++);
}



void Write_GLCD(unsigned int data)
{
unsigned char temp;

temp=data/1000;
Glcd_PutChar(temp+'0');
data=data-temp*1000;
temp=data/100;
Glcd_PutChar(temp+'0');
data=data-temp*100;
temp=data/10;
Glcd_PutChar(temp+'0');
data=data-temp*10;
Glcd_PutChar(data+'0');
}

void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{
	
    sirovi0 = ADCBUF0;
    sirovi1 = ADCBUF1;
    sirovi2 = ADCBUF2;
    
    temp0 = sirovi0;
    temp1 = sirovi1;
    temp2 = sirovi2;
    
    IFS0bits.ADIF = 0;
    
}