/*  Lab 2 - Main C file for Clock program

    Computerarchitektur 3
    (C) 2018 J. Friedrich, W. Zimmermann
    Hochschule Esslingen

    Author:  Liam Huppert, Mohammad Rshdan May 2024
*/

#include <hidef.h>                              // Common defines
#include <mc9s12dp256.h>                        // CPU specific defines


#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"

#define SELECT12HOURS 0

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
    PORTB       = 0x80;
    ampm[0] = 'a';
    ampm[1] = 'm';
}





   // Start Thermometer
// ****************************************************************************
// Global variables

char temperature[5];
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
        lcd[11] = '-';                               //the 11th byte of the lcd array is the sign of the temperature
      } else {
        lcd[11] = ' ';       
      }
       asm{
            ldd temp                                 //we load the temperature into d register
            ldx #bufferLocation                      //as well as the adress for our bufferLocation
            
            JSR decToASCII                           //so we can convert the temperature to a decimal value
      }
      
      lcd[12] = bufferLocation[4];                   //now all we need to do is grab the bytes we need to display 
      lcd[13] = bufferLocation[5];
      lcd[14] = '°';
      lcd[15] = 'C';                                 //and append °C to the string
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


void incHours(void) {                                //this function handles incrementing the hours of our clock
    hours++;                                         //of course we start by adding 1 to hours
    if(SELECT12HOURS == 0) {                         //now we need to look at two cases, one is the 24 hour clock
        if(hours == 24) {                            //24 hour clock is relativly simple, if we reach 24 hours
            hours = 0;                               //we simply reset
        } 
    } else {                                         //the other case is the 12 hours am/pm clock
        if(ampm[0] == 'a') {                         //first we look whether we are at am or pm. If we are at am
            if(hours == 12) {                        //and the clock reaches 12
                ampm[0] = 'p';                       //we turn from am to pm
            }else if(hours == 13) {                  //if the clock reaches 13 
                hours = 1;                           //we jump back down to 1
            }      
        } else {                                     //if we are at pm
            if(hours == 12) {                        //and the clock reaches 12
                ampm[0] = 'a';                       //we turn form pm to am
            }else if(hours == 13) {                  //if the clocke reaches 13
                hours = 1;                           //we jump back down to 1
          }  
        }
    }
}
void incMinutes(void) {                              //this function handles incrementing the minutes of our clock
    minutes++;                                       //of course we start by adding 1 to minutes
    if(minutes < 60) {                               //if minutes reaches 60
        return;  
    }
    minutes = 0;                                     //we reset
    incHours();                                      //and call incHours to handle the "overflow"
}

void incrementTime(void) {                           //this function handles incrementing the seconds of our clock
    seconds++;                                       //of course we start by adding 1 to seconds
    if(seconds < 60) {                               //if seconds reaches 60
        return;
    }
    seconds = 0;                                     //we reset
    incMinutes();                                    //and call incMinutes to handle the "overflow"
}
  

void buildTimeString(void) {                         //this function builds the string to be displayed on the lcd
      
      asm{
            ldd hours
            ldx #bufferLocation                      //first we need the hours as decimal character
            
            JSR decToASCII
              
      }

      lcd[0] = bufferLocation[4];                    //then we simply grab the 4th and 5th byte
      lcd[1] = bufferLocation[5];                    //which represent the 10s and 1s of the String
      lcd[2] = ':';                                  //and append a :

      asm{
            ldd minutes                              //now we need the minutes as decimal character
            JSR decToASCII
              
      }

      lcd[3] = bufferLocation[4];                    //again we grab the 4th and 5th byte
      lcd[4] = bufferLocation[5];                    
      lcd[5] = ':';                                  //and append a :
      asm{
            ldd seconds                              //last thing we need is the seconds
            JSR decToASCII
              
      }
      lcd[6] = bufferLocation[4];                    //and once again the 4th and 5th byte
      lcd[7] = bufferLocation[5];
      if(SELECT12HOURS == 1){                        //now if we are in the 12 hour clock mode
        lcd[8] = ampm[0];                            //the 8th and 9th byte are am/pm
        lcd[9] = ampm[1];  
      }else {                                        //otherwise we just add spaces
        lcd[8] = ' ';
        lcd[9] = ' ';  
      }
      
      
      lcd[10] = ' ';

}
  

void changeLCDNames(void) {                          //this function toggles the lcd names every 10 seconds
      if(counter++ == 10){                           //if we reach 10
           counter = 0;                              //we can reset counter
           if(toggle == 0) {                         //now we have toggle acting as a boolean whether we display the names or the Copyright
               toggle = 1;
               WriteLine_Wrapper("(C) IT SS2024", 1); //in one case we write the Copyright             
           } else if(toggle == 1) {
               toggle = 0;
               WriteLine_Wrapper("Liam + Mohammad", 1); //in the other the names of the members of our group             
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
    initThermo();
    initTicker();                               // Initialize the time ticker
    
                                         
    for(;;){                                    // Endless loop
        if (clockEvent){
            if((PORTB & 0x80)) {                //depending whether we are in the simulator or on hardware this boolean needs to be inverted
    	      incrementTime();   
    	    }	   
    	    
    	    clockEvent = 0;                     //we reset the interrupt
    	    updateThermo();                     //we update the temperature
            buildTimeString();                  //we assemble our string for the lcd
            WriteLine_Wrapper(lcd , 0);         //so we can display it
            changeLCDNames();                   //and at last we toggle our names on the first line of the lcd
        }
    }
}