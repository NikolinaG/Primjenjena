#include "serijska.h"
#include<p30fxxxx.h>
#include <stdlib.h>
#include<p30f4013.h>
#include "funkcije.h"



void initUART1(void)
{
    U1BRG=0x0040;//ovim odredjujemo baudrate

    U1MODEbits.ALTIO=0;//biramo koje pinove koristimo za komunikaciju osnovne ili alternativne

    IEC0bits.U1RXIE=1;//omogucavamo rx1 interupt

    U1STA&=0xfffc;

    U1MODEbits.UARTEN=1;//ukljucujemo ovaj modul

    U1STAbits.UTXEN=1;//ukljucujemo predaju
}


    
 

void RS232_putst(register const char *str)
{
    while ((*str)!=0)
    {
        WriteUART1(*str);
        if(*str==13) WriteUART1(13);
        if(*str==10) WriteUART1(10);
        
        str++;
    }
}

void WriteUART1(unsigned int data)
{
	  while(!U1STAbits.TRMT);// da li je pretgodno slanje zavrseno

    if(U1MODEbits.PDSEL == 3)//da li ima deveti bit
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}

void WriteUART1dec2string(unsigned int data)
{
	unsigned char temp;

	temp=data/1000;
	WriteUART1(temp+'0');
	data=data-temp*1000;
	temp=data/100;
	WriteUART1(temp+'0');
	data=data-temp*100;
	temp=data/10;
	WriteUART1(temp+'0');
	data=data-temp*10;
	WriteUART1(data+'0');
}