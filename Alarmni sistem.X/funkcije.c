#include<p30fxxxx.h>
#include <stdlib.h>
#include<p30f4013.h>
#include "tajmer.h"
//#include <delay.h>
#include "adc.h"
#include "driverGLCD.h"
#include "serijska.h"
#include <string.h>

unsigned int stoperica,stoperica1=0,stoperica_ms=0,set_pir=0,stoperica_mq3=0;


unsigned int X, Y,x_vrednost, y_vrednost;

int sd=0;
int sirovi1,sirovi0,sirovi2;
int broj,broj1,broj2,temp0,temp1,temp2,temp;
int i=0,j,p=60,c=0,l;
char niz[4],niz1[4], ni[4]={'1','2','3','5'};


const unsigned int AD_Xmin =63;
const unsigned int AD_Xmax =3600;
const unsigned int AD_Ymin =189;
const unsigned int AD_Ymax =3450;

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

void Touch_Panel (void)
{
// vode horizontalni tranzistori
	DRIVE_A = 1;  
	DRIVE_B = 0;
    
     LATCbits.LATC13=1;
     LATCbits.LATC14=0;

	Delay(500); //cekamo jedno vreme da se odradi AD konverzija
				
	// ocitavamo x	
	x_vrednost = temp0;//temp0 je vrednost koji nam daje AD konvertor na BOTTOM pinu		

	// vode vertikalni tranzistori
     LATCbits.LATC13=0;
     LATCbits.LATC14=1;
	DRIVE_A = 0;  
	DRIVE_B = 1;

	Delay(500); //cekamo jedno vreme da se odradi AD konverzija
	
	// ocitavamo y	
	y_vrednost = temp1;// temp1 je vrednost koji nam daje AD konvertor na LEFT pinu	
	
//Ako želimo da nam X i Y koordinate budu kao rezolucija ekrana 128x64 treba skalirati vrednosti x_vrednost i y_vrednost tako da budu u opsegu od 0-128 odnosno 0-64
//skaliranje x-koordinate

    X=(x_vrednost-63)*0.0361888606;



//X= ((x_vrednost-AD_Xmin)/(AD_Xmax-AD_Xmin))*128;	
//vrednosti AD_Xmin i AD_Xmax su minimalne i maksimalne vrednosti koje daje AD konvertor za touch panel.


//Skaliranje Y-koordinate
	Y= ((y_vrednost-189)*0.0196258816);

//	Y= ((y_vrednost-AD_Ymin)/(AD_Ymax-AD_Ymin))*64;
}

void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{
	
    sirovi0 = ADCBUF0;
    sirovi1 = ADCBUF1;
   // sirovi2 = ADCBUF2;
    
    temp0 = sirovi0;
    temp1 = sirovi1;
    //temp2 = sirovi2;
    
    IFS0bits.ADIF = 0;
    
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


void Delay_50u (int vreme){
        stoperica = 0;
		while(stoperica < vreme);
}

void Delay_500u (int vreme){
        stoperica_mq3 = 0;
		while(stoperica_mq3 < vreme);
}


void __attribute__ ((__interrupt__)) _T2Interrupt(void) // svakih 500us
{

	TMR2 = 0;
    stoperica_mq3++;
    stoperica1++;
	IFS0bits.T2IF = 0; 
    
   
}

void __attribute__ ((__interrupt__)) _T1Interrupt(void) // svakih 50u
{
	TMR1 = 0;
    stoperica++;//brojac za funkciju Delay_50u
    stoperica_ms++;
    if(stoperica_ms>1 && set_pir==1){
        LATAbits.LATA11=1;
    }
    if(stoperica_ms>6 && set_pir==1){
        LATAbits.LATA11=0;
        stoperica_ms=0;
    }
    
    IFS0bits.T1IF = 0;
}
    
    
void initBrava(){
	TRISDbits.TRISD9=0;//konfigurisemo kao izlaz
    Init_T2();
    
}

void brava(int x){
    int j;
    if(x==0){
        for(j=0;j<20;j++)
        {
            LATDbits.LATD9=1;		
            Delay_50u(12);
            LATDbits.LATD9=0;
            Delay_50u(388);
            LATDbits.LATD9=1;  
        }
}else if(x==1){
        for(j=0;j<20;j++)
        {
        LATDbits.LATD9=1;		
        Delay_50u(28);
        LATDbits.LATD9=0;
        Delay_50u(372);
        LATDbits.LATD9=1;
        }
}else{
        for(j=0;j<20;j++)
            {
            LATDbits.LATD9=1;		
            Delay_50u(48);
            LATDbits.LATD9=0;
            Delay_50u(352);
            LATDbits.LATD9=1;
            }
}
}

void init_interrupt(void){
    INTCON2bits.INT1EP=0; //na rastucu ivicu
    IFS1bits.INT1IF=0; //reset flag interrupt INT
}

void ukljuciPIR(int x){
    if(x==1){
        IEC1bits.INT1IE=1;
    }else{
        IEC1bits.INT1IE=0;
    }
}

void initPiezo(){
    LATAbits.LATA11=0;
    TRISAbits.TRISA11=0;//konfigurisemo kao izlaz	
}
void ukljuciPiezo(int x){
    if(x==1){
        set_pir=1;
        LATBbits.LATB10=1; // ukljucenje osvetljenja displeja
    }else{
        set_pir=0;
    }
}

void __attribute__((__interrupt__))_INT1Interrupt(void)
    {
    IEC1bits.INT1IE=0;
    IFS1bits.INT1IF=0;
  
    ukljuciPiezo(1);
    
    IEC1bits.INT1IE=1;
    IFS1bits.INT1IF=0;
    }	
   
void displej(){
    
    GLCD_Rectangle(6,4,29,17);
    GLCD_Rectangle(37,4,60,17);
    GLCD_Rectangle(68,4,91,17);
    
    GLCD_Rectangle(6,20,29,33);
    GLCD_Rectangle(37,20,60,33);
    GLCD_Rectangle(68,20,91,33);
    
    GLCD_Rectangle(6,36,29,49);
    GLCD_Rectangle(37,36,60,49);
    GLCD_Rectangle(68,36,91,49);
    
    GLCD_Rectangle(99,20,122,33); // pravougaonik za "Delete"
    GLCD_Rectangle(99,36,122,49);
    
    GoToXY(108,3);		
    GLCD_Printf ("C");
    GoToXY(105,5);		
    GLCD_Printf ("OK");
    
    
    GoToXY(15,1);		
    GLCD_Printf ("1");
    
    GoToXY(46,1);		
    GLCD_Printf ("2");
    
    GoToXY(77,1);		
    GLCD_Printf ("3");
    
    GoToXY(15,3);		
    GLCD_Printf ("4");
    
    GoToXY(46,3);		
    GLCD_Printf ("5");
    
    GoToXY(77,3);		
    GLCD_Printf ("6");
    
    GoToXY(15,5);		
    GLCD_Printf ("7");
    
    GoToXY(46,5);		
    GLCD_Printf ("8");
    
    GoToXY(77,5);		
    GLCD_Printf ("9");
    
    GoToXY(6,7);		
    GLCD_Printf ("Password: ");
    

}

void tasteri(){
    if ((12<X)&&(X<34) && (48<Y)&&(Y<59) && p<96){
        GoToXY(p,7);		
        GLCD_Printf ("1");
        p=p+9;
        niz[i]='1';
        i++;
    }  
    
    if ((41<X)&&(X<61) && (48<Y)&&(Y<59) && p<96){
        GoToXY(p,7);		
        GLCD_Printf ("2");
        p=p+9;
        niz[i]='2';
        i++;
    }
    if ((69<X)&&(X<90) && (48<Y)&&(Y<59) && p<96){
        GoToXY(p,7);		
        GLCD_Printf ("3");
        p=p+9;
        niz[i]='3';
        i++;
    }
    if ((12<X)&&(X<34) && (32<Y)&&(Y<42) && p<96){
        GoToXY(p,7);		
        GLCD_Printf ("4");
        p=p+9;
        niz[i]='4';
        i++;
    }
    if ((41<X)&&(X<61) && (32<Y)&&(Y<42) && p<96){
        GoToXY(p,7);		
        GLCD_Printf ("5");
        p=p+9;
        niz[i]='5';
        i++;
    }
    if ((69<X)&&(X<90) && (32<Y)&&(Y<42) && p<96){
        GoToXY(p,7);		
        GLCD_Printf ("6");
        p=p+9;
        niz[i]='6';
        i++;
    }
    if ((12<X)&&(X<34) && (18<Y)&&(Y<30) && p<96){
        GoToXY(p,7);		
        GLCD_Printf ("7");
        p=p+9;
        niz[i]='7';
        i++;
    }
    if ((41<X)&&(X<61) && (18<Y)&&(Y<30) && p<96){
        GoToXY(p,7);		
        GLCD_Printf ("8");
        p=p+9;
        niz[i]='8';
        i++;
    }
    if ((69<X)&&(X<90) && (18<Y)&&(Y<30) && p<96){
        GoToXY(p,7);		
        GLCD_Printf ("9");
        p=p+9;
        niz[i]='9';
        i++;
    }
    

    if ((100<X)&&(X<118) && (32<Y)&&(Y<42)){
        GoToXY(60,7);
        GLCD_Printf ("         ");  
        p=60;
        i=0;
    }
    
    
    if ((100<X)&&(X<118) && (18<Y)&&(Y<30)){

                if(strncmp(ni,niz,4)==0){ 
                    GoToXY(60,7);		
                    GLCD_Printf ("Correct!");
                    ukljuciPiezo(0);
                    Delay_500u(5000); // nakon 2.5s se gubi Try again
                    GoToXY(60,7);
                    GLCD_Printf ("         ");  
                    i=0;
                    p=60;
                    Delay_500u(10000); // nakon 5s se iskljucuje displej
                    LATBbits.LATB10=0;
                }else{
                    GoToXY(60,7);		
                    GLCD_Printf ("Try again!");
                     Delay_500u(5000); // nakon 2.5s se gubi Try again
                    GoToXY(60,7);
                    GLCD_Printf ("          ");
                    i=0;
                    p=60;
                }
        j=0;
    }
   
   while(X<1000){
       Touch_Panel();
       //Delay_50u(1);
    }
   
} 

void initSenzorDima(){
    ADPCFGbits.PCFG12=1;//konfigurisemo kao digitalni
    TRISBbits.TRISB12=1;//konfigurisemo kao ulaz	
}

void senzorDima(){
    
    if(PORTBbits.RB12==0){
        sd=1;
    }
    if(sd==1){
         ukljuciPiezo(1);
         brava(1); // poluotvorena vrata
         sd=0;    
    }
    
}

void promenaSifre(char *nizz){
    for(l=0;l<4;l++)
        ni[l]='\0';
    
    for(c=0;c<=3;c++)
    {
        ni[c]=nizz[c];
    }
}