#include<p30fxxxx.h>

void ConfigureTSPins(void);
void Delay(unsigned int N);
void Write_GLCD(unsigned int data);
void Touch_Panel (void);
void Delay_50u (int vreme);
void Delay_500u (int vreme);
void initBrava();
void brava(int);
void init_interrupt(void);
void ukljuciPIR(int);
void initPiezo();
void ukljuciPiezo(int);
void initSenzorDima();
void senzorDima();
void displej();
void sifra(int taster);
void tasteri();
void promenaSifre(char *);
