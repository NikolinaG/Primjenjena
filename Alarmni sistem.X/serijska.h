#include <stdio.h>
#include <stdlib.h>
#include<p30fxxxx.h>

void initUART1(void);
void RS232_putst(register const char *str);
void WriteUART1(unsigned int data);
void WriteUART1dec2string(unsigned int data);
void ukljuciSistem();
void iskljuciSistem();