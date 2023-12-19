/* 
 * File:   main.c
 * Author: Kristina Nikolic
 *         Stasa Vucinic
 *         Laslo Halapi
Naziv programa: PROJEKAT IZ PREDMETA PRIMENJENA ELEKTRONIKA
Opis programa:  Alarmni sistem za ku?u izra?en upotrebom razvojne 
 *              plo?e EasyPIC v7 firme Mikroelektronika sa mikrokontrolerom 
 *              dsPIC30f4013, grafi?kim ekranom osetljivim na dodir, 
 *              serijskom komunikacijom RS232 i Piezo buzzer-om.
 *              Pored toga, za simulaciju otvaranja vrata kori??en 
 *              je servo motor SG90, detekcija prevelike koli?ine 
 *              ugljen-dioksida u ku?i ostvarena je pomo?u senzora MQ3,
 *              dok je detekcija pokreta registrovana pomo?u PIR senzora.
 *
 * Created on 17. december 2017., 11.04
 */
#include<p30fxxxx.h>
#include <stdlib.h>
#include "driverGLCD.h"
#include<p30f4013.h>
#include "adc.h"
#include "funkcije.h"
#include "serijska.h"
#include "tajmer.h"


_FOSC(CSW_ON_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal//
_FWDT(WDT_OFF);
_FGS(CODE_PROT_OFF);

unsigned char tempRX;
char string[20],novas[4];
int u=0, br, f,r,stanje1=0,stanje2=0,stanje3=0,stanje4=0,o, stanje5=0, uartf=0;

void __attribute__((__interrupt__)) _U1RXInterrupt(void) 
{
    IFS0bits.U1RXIF = 0;
    tempRX = U1RXREG;
    
    if(u<16)
    {
        string[u]=tempRX;
        u++;
    }
    else
        br=1;
}

void main(void)
{
    ADCinit();
	ConfigureADCPins();
	ADCON1bits.ADON=1;
    
    Init_T1();
    Init_T2();
    init_interrupt();
    
    initUART1();
    
    ConfigureLCDPins();
	ConfigureTSPins();

	GLCD_LcdInit();
    GLCD_ClrScr();
    displej();
       
    ADPCFGbits.PCFG10=1; 
    TRISBbits.TRISB10=0;
    LATBbits.LATB10=0; // iskljucuje pozadinsko osvetljenje displeja
    
    initBrava();
    brava(0); // pocetno stanje za vrata je zatvoreno
    stanje4=1;
 
    initSenzorDima();
    initPiezo();
    
     
	while(1)
	{
        
    Touch_Panel ();
    
    tasteri();
    
    senzorDima(); 
    
    if(string[0]=='S' && string[1]=='T'&& string[2]=='A' && string[3]=='R' && string[4]=='T' && string[5]==' ' && string[6]=='A'&& string[7]=='L' && string[8]=='A' && string[9]=='R' && string[10]=='M' && br==0 )
            {
                
                if(stanje1==1)
                {
                    RS232_putst("Alarm is already on!!! \n");
                }
                else{
                stanje1 = 1;
                RS232_putst("Alarm is on!!! \n");
                stanje2=0;
                IFS1bits.INT1IF=0;
                ukljuciPIR(1);
                }
                for(f=0;f<15;f++)
                    string[f]='\0';
                u=0;

    }
    
    else if(string[0]=='S' && string[1]=='T' && string[2]=='O' && string[3]=='P' && string[4]==' ' && string[5]=='A' && string[6]=='L'&& string[7]=='A' && string[8]=='R' && string[9]=='M' && br==0 )
            {
                
                if(stanje2==1)
                {
                    RS232_putst("Alarm is already off!!! \n");
                }
                else{
                    stanje2=1;
                RS232_putst("Alarm is off!!! \n");
                stanje1=0;
                ukljuciPIR(0);
               
                }
                 for(f=0;f<15;f++)
                    string[f]='\0';
                u=0;

    }
    
    else if(string[0]=='O' && string[1]=='P' && string[2]=='E' && string[3]=='N' && string[4]==' ' && string[5]=='T' && string[6]=='H'&& string[7]=='E' && string[8]==' ' && string[9]=='D' && string[10]=='O' && string[11]=='O' && string[12]=='R' && br==0 )
            {
                
                if(stanje3==1)
                {
                    RS232_putst("The door is already open!!! \n");
                }
                else{
                    stanje3=1;
                RS232_putst("The door is opened!!! \n");
                stanje4=0;
                brava(2);
                }
                for(f=0;f<15;f++)
                    string[f]='\0';
                u=0;

    }
    
    else if(string[0]=='C' && string[1]=='L' && string[2]=='O' && string[3]=='S' && string[4]=='E' && string[5]==' ' && string[6]=='T'&& string[7]=='H' && string[8]=='E' && string[9]==' ' && string[10]=='D' && string[11]=='O' && string[12]=='O' && string[13]=='R' && br==0 )
            {
                
                if(stanje4==1)
                {
                    RS232_putst("The door is already closed!!! \n");
                }
                else{
                    stanje4=1;
                RS232_putst("The door is closed!!! \n");
                stanje3=0;
                brava(0);
                }
                for(f=0;f<15;f++)
                    string[f]='\0';
                u=0;

    }
      
    else if((stanje5==1) || (string[0]=='C' && string[1]=='H' && string[2]=='A' && string[3]=='N' && string[4]=='G' && string[5]=='E' && string[6]==' ' && string[7]=='P' && string[8]=='A' && string[9]=='S' && string[10]=='S' && string[11]=='W' && string[12]=='O' && string[13]=='R' && string[14]=='D' && br==0 ))
            {
        Delay_500u(100);
                if(stanje5==1)
                {
                    if(48<string[0] && string[0]<58 && 48<string[1] && string[1]<58 && 48<string[2] && string[2]<58 && 48<string[3] && string[3]<58 && string[4]<31){
                           Delay_500u(100);
                            for(r=0;r<4;r++)
                                novas[r]='\0';
                            
            
                           
                               Delay_500u(100);
                            for(o=0;o<=3;o++)
                                {
                                    novas[o]=string[o];
                                } 
                                 Delay_500u(100);
                                promenaSifre(novas);                               
                                RS232_putst("Password is changed !!! \n");
                                stanje5=0;
                                
                    
                    }else if(string[0]>31){
                        RS232_putst("Password must contain four numbers (from 1 to 9)!!! Try again:");
                    }
                }
                else{
                RS232_putst("Please enter new password: ");
                stanje5=1;
                }

                for(f=0;f<15;f++)
                    string[f]='\0';
                u=0;

    }
    else if(stanje5==0 && string[0]>31 && string[0]!='C' && string[1]!='H' && string[2]!='A' && string[3]!='N' && string[4]!='G' && string[5]!='E' && string[6]!=' ' && string[7]!='P' && string[8]!='A' && string[9]!='S' && string[10]!='S' && string[11]!='W' && string[12]!='O' && string[13]!='R' && string[14]!='D' && string[0]!='C' && string[1]!='L' && string[2]!='O' && string[3]!='S' && string[4]!='E' && string[5]!=' ' && string[6]!='T'&& string[7]!='H' && string[8]!='E' && string[9]!=' ' && string[10]!='D' && string[11]!='O' && string[12]!='O' && string[13]!='R' && string[0]!='O' && string[1]!='P' && string[2]!='E' && string[3]!='N' && string[4]!=' ' && string[5]!='T' && string[6]!='H'&& string[7]!='E' && string[8]!=' ' && string[9]!='D' && string[10]!='O' && string[11]!='O' && string[12]!='R' && string[0]!='S' && string[1]!='T' && string[2]!='O' && string[3]!='P' && string[4]!=' ' && string[5]!='A' && string[6]!='L'&& string[7]!='A' && string[8]!='R' && string[9]!='M' && string[0]!='S' && string[1]!='T' && string[2]!='A' && string[3]!='R' && string[4]!='T' && string[5]!=' ' && string[6]!='A'&& string[7]!='L' && string[8]!='A' && string[9]!='R' && string[10]!='M' && br==0){       
       Delay_500u(1000);
       RS232_putst("Something is wrong! \n");
        for(f=0;f<15;f++)
                    string[f]='\0';
                u=0;
                
    }
   
	}//while

}//main
	
												
