/*  Lab 2 - Main C file for Clock program

    Computerarchitektur 3
    (C) 2018 J. Friedrich, W. Zimmermann
    Hochschule Esslingen

    Author:  W.Zimmermann, July 19, 2017
*/

#include <hidef.h>                              // Common defines
#include <mc9s12dp256.h>                        // CPU specific defines

#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"


enum clockModes {
      NORMALMODE,
      SETMODE
};

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
}




// ****************************************************************************
// Global variables
unsigned char clockEvent = 0;
unsigned int hours = 11;
unsigned int minutes = 59;
unsigned int seconds = 30;
unsigned int counter = 0;
unsigned int toggle = 0;
char lcdClock[9];
char bufferLocation[8];
enum clockModes clockMode = NORMALMODE;


void incrementTime(void) {
    seconds++;
    if(seconds < 60) {
        return;
    }
    seconds = 0;
    minutes++;
    if(minutes < 60) {
        return;
    }
    minutes = 0;
    hours++;
    if(hours == 24) {
        hours = 0; 
    }
}

void incHours(void) {
    hours++;
    if(hours == 24) {
        hours = 0;   
    }
}
void incMinutes(void) {
    minutes++;
    if(minutes < 60) {
        return;  
    }
    minutes = 0;
    hours++;
    if(hours == 24) {
        hours = 0; 
    }
}
  

void buildTimeString(void) {
      
      asm{
            ldd hours
            ldx #bufferLocation
            
            JSR decToASCII
              
      }
      lcdClock[0] = bufferLocation[4];
      lcdClock[1] = bufferLocation[5];
      lcdClock[2] = ':';
      asm{
            ldd minutes
            JSR decToASCII
              
      }
      lcdClock[3] = bufferLocation[4];
      lcdClock[4] = bufferLocation[5];
      lcdClock[5] = ':';
      asm{
            ldd seconds
            JSR decToASCII
              
      }
      lcdClock[6] = bufferLocation[4];
      lcdClock[7] = bufferLocation[5];
      lcdClock[8] = '\0';

}


// ****************************************************************************
void main(void) 
{   EnableInterrupts;                           // Global interrupt enable

    initLED_C();                    		// Initialize the LEDs

    initLCD();                    		// Initialize the LCD
    WriteLine_Wrapper("Clock Template", 0);
    WriteLine_Wrapper("(C) HE Prof. Z", 1);    

    initTicker();                               // Initialize the time ticker
    
    for(;;)                                     // Endless loop
    {   if (clockEvent)
    	{
    	  if (clockMode == NORMALMODE) {
    	    incrementTime();
    	     if(PORTB & 0x80) {
    	         clockMode = SETMODE;   
    	     }
    	  }else {
    	        
    	     if(!(PORTB & 0x80)) {
    	         clockMode = NORMALMODE;   
    	     }  
    	  }	   
    	    
    	    clockEvent = 0;
         
          buildTimeString();
          WriteLine_Wrapper(lcdClock , 0);
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
    }
}



