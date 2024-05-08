
 /*
   Author: Mohamma Rshdan, Laim
   Returns temperature  between -30 and +70 °C in cast String
*/

#include <hidef.h>                              // Common defines
#include <mc9s12dp256.h>                        // CPU specific defines
#include "thermometer.h"
//#include "clock.h"
//#include "main.h"

#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"

//Global variables
char temperature[6] = "   °C";
unsigned int value = 0x3FF;      // Measurement value
int temp = 70;                 // for simulation debugging


void decToASCII(void);

// Assembler decToASCII_Wrapper_Thermo
void decToASCII_Wrapper_Thermo(char *txt, int value)
{   
  asm
    {  	LDX txt
        LDD value
        JSR decToASCII 
    }
}


// {copied from Sample ADCInterruptC.mcp }
// --- ADC interrupt service routine -----------------------       
void interrupt 22 adcISR(void)
{  // Read the result registers and compute average of 4 measurements
   value = (ATD0DR0 + ATD0DR1 + ATD0DR2 + ATD0DR3) >> 2;                   
   
   ATD0CTL2 = ATD0CTL2 | 0x01;  // Reset interrupt flag (bit 0 in ATD0CTL2)

   ATD0CTL5 = 0b10000111;       // Start next measurement on single channel 7
}

/***************************************************************
Public interface function: void initThermo() 

...One Time Configuration via Control Registers

Parameter: -
Return:    -
*/
void initThermo(void){

      ATD0CTL2 = 0b11000010;// Enable ATD0, enable interrupt
      ATD0CTL3 = 0b00100000;// Sequence: 4 measurements
      ATD0CTL4 = 0b00000101;// 10bit, 2MHz ATD0 clock

      ATD0CTL5 = 0b10000111;// Start first measurement on single channel 7
     
}
// {copied from Sample ADCInterruptC.mcp }


/***************************************************************
Public interface function: void updateThermo() 

... checks temperature if positive or negative and convert the temperature value to a string

Parameter: -  
Return:    -
*/
void updateThermo(void){
      //temp = ((value*50)/512) - 30;
      if(temp < 0) {
        temperature[0] = '-'; // define signature
      } else {
        temperature[0] = ' '; // define signature      
      }
       decToASCII_Wrapper_Thermo((temperature+1), temp);        // *temeperature points auf temperature[0] + 1 = temeperature[1] also signature = temperature[0] is defined
}

/***************************************************************
Public interface function: char* getTemperature() 

Parameter: -
Return:    char* : string temperatur
*/
char * getTemperature(){
  return temperature;
}