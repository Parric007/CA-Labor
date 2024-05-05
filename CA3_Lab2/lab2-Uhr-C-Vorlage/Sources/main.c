/*  Lab 2 - Main C file for Clock program

    Computerarchitektur 3
    (C) 2018 J. Friedrich, W. Zimmermann
    Hochschule Esslingen

    Author:  W.Zimmermann, July 19, 2017
*/

#include <hidef.h>                              // Common defines
#include <mc9s12dp256.h>                        // CPU specific defines
//#include "thermometer.h"


#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"

#define SELECT12HOURS 1

char ampm[2];

// PLEASE NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!:
// Files lcd.asm and ticker.asm do contain SOFTWARE BUGS. Please overwrite them
// with the lcd.asm file, which you bug fixed in lab 1, and with file ticker.asm
// which you bug fixed in prep task 2.1 of this lab 2.
//
// To use decToASCII you must insert file decToASCII from the first lab into
// this project
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


// ****************************************************************************
// Function prototype(s)
// Note: Only void Fcn(void) assembler functions can be called from C directly.
//       For non-void functions a C wrapper function is required.
void initTicker(void);


// Prototypes and wrapper functions for dec2ASCII (from lab 1)
void decToASCII(void);

void decToASCII_Wrapper(char *txt, int val)
{   asm
    {  	LDX txt
        LDD val
        JSR decToASCII
    }
}

// Prototypes and wrapper functions for LCD driver (from lab 1)
void initLCD(void);
void writeLine(void);

void WriteLine_Wrapper(char *text, char line)
{   asm
    {	LDX  text
        LDAB line
        JSR  writeLine
    }
}

void checkButtonsNormalMode(void);
void checkButtonsSetMode(void);

// ****************************************************************************

void initLED_C(void)
{   DDRJ_DDRJ1  = 1;	  	// Port J.1 as output
    PTIJ_PTIJ1  = 0;		
    DDRB        = 0xFF;		// Port B as output
    PORTB       = 0x0;
    ampm[0] = 'a';
    ampm[1] = 'm';
}





   // Start Thermometer
// ****************************************************************************
// Global variables
//char temperature[6] = "   °C";
char temperature[5];
char test[4];
unsigned int value = 0x3FF;      // Measurement value
int temp = 70;                 // for simulation debugging
char bufferLocation[8];
char lcd[17];

// Assembler


/***************************************************************/
// start copy from Sample ADCInterruptC.mcp
// --- ADC interrupt service routine -----------------------       
void interrupt 22 adcISR(void)
{  // Read the result registers and compute average of 4 measurements
   value = (ATD0DR0 + ATD0DR1 + ATD0DR2 + ATD0DR3) >> 2;                   
   
   ATD0CTL2 = ATD0CTL2 | 0x01;  // Reset interrupt flag (bit 0 in ATD0CTL2)

   ATD0CTL5 = 0b10000111;       // Start next measurement on single channel 7
}

void initThermo(void){

      ATD0CTL2 = 0b11000010;// Enable ATD0, enable interrupt
      ATD0CTL3 = 0b00100000;// Sequence: 4 measurements
      ATD0CTL4 = 0b00000101;// 10bit, 2MHz ATD0 clock

      ATD0CTL5 = 0b10000111;// Start first measurement on single channel 7
     
}
// End copy from Sample ADCInterruptC.mcp
/****************************************************************/

void updateThermo(void){
      //temp = ((value*50)/512) - 30;                // temperature calculation when chip
      if(temp < 0) {
        lcd[11] = '-'; 
      } else {
        lcd[11] = ' ';       
      }
       asm{
            ldd temp
            ldx #bufferLocation
            
            JSR decToASCII 
      }
      
      lcd[12] = bufferLocation[4];
      lcd[13] = bufferLocation[5];
      lcd[14] = '°';
      lcd[15] = 'C';
      lcd[16] = '\0';
}


char * getTemperature(){                             // get Temperature
  
  return temperature;
}

// ****************************************************************************
       // End Thermometer





// ****************************************************************************
// Global variables
unsigned char clockEvent = 0;
unsigned int hours = 11;
unsigned int minutes = 59;
unsigned int seconds = 30;
unsigned int counter = 0;
unsigned int toggle = 0;





void incHours(void) {
    hours++;
    if(SELECT12HOURS == 0) {
        if(hours == 24) {
            hours = 0; 
        } 
    } else {
        if(ampm[0] == 'a') {
            if(hours == 12) {
                ampm[0] = 'p';
            }else if(hours == 13) {
                hours = 1;
            }      
        } else {
            if(hours == 12) {
                ampm[0] = 'a';
            }else if(hours == 13) {
                hours = 1;
          }  
        }
    }
}
void incMinutes(void) {
    minutes++;
    if(minutes < 60) {
        return;  
    }
    minutes = 0;
    incHours();
}

void incrementTime(void) {
    seconds++;
    if(seconds < 60) {
        return;
    }
    seconds = 0;
    incMinutes(); 
    
}
  

void buildTimeString(void) {
      
      asm{
            ldd hours
            ldx #bufferLocation
            
            JSR decToASCII
              
      }
      lcd[0] = bufferLocation[4];
      lcd[1] = bufferLocation[5];
      lcd[2] = ':';
      asm{
            ldd minutes
            JSR decToASCII
              
      }
      lcd[3] = bufferLocation[4];
      lcd[4] = bufferLocation[5];
      lcd[5] = ':';
      asm{
            ldd seconds
            JSR decToASCII
              
      }
      lcd[6] = bufferLocation[4];
      lcd[7] = bufferLocation[5];
      if(SELECT12HOURS == 1){
        lcd[8] = ampm[0];
        lcd[9] = ampm[1];  
      }else {
        lcd[8] = ' ';
        lcd[9] = ' ';  
      }
      
      
      lcd[10] = ' ';

}

void changeLCDNames(void) {
      if(counter++ == 10){
           counter = 0;
           if(toggle == 0) {
               toggle = 1;
               WriteLine_Wrapper("(C) IT SS2024", 1);              
           } else if(toggle == 1) {
               toggle = 0;
               WriteLine_Wrapper("Liam + Mohammad", 1);              
           }  
      }  
}


// ****************************************************************************
void main(void) 
{   EnableInterrupts;                           // Global interrupt enable

    initLED_C();                    		// Initialize the LEDs

    initLCD();                    		// Initialize the LCD
    WriteLine_Wrapper("Clock Template", 0);
    WriteLine_Wrapper("Liam + Mohammad", 1);    

    initTicker();                               // Initialize the time ticker
    
    for(;;)                                     // Endless loop
    {  
        if (clockEvent){
            

    	  if(!(PORTB & 0x80)) {
    	      incrementTime();   
    	  }	   
    	    
    	    clockEvent = 0;
    	    updateThermo();
          buildTimeString();
          WriteLine_Wrapper(lcd , 0);

          changeLCDNames();
           }
    }
}