/* 
  Prototypes for thermometer.c
  Author: Mohamma Rshdan, Laim
*/

#ifndef THERMOMETHER_HEADER
#define THERMOMETHER_HEADER

// Function prototype(s)
void initThermo(void);
void updateThermo(void);
void decToASCII_Wrapper_Thermo(char *,int);
void interrupt 22 adcISR(void);
char * getTemperature(void);

#endif