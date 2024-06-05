/*  Radio signal clock - DCF77 Module

    Computerarchitektur 3
    (C) 2018 J. Friedrich, W. Zimmermann Hochschule Esslingen

    Author:   W.Zimmermann, Jun  10, 2016
    Modified: -
*/

/*
; A C H T U N G:  D I E S E  S O F T W A R E  I S T  U N V O L L S T � N D I G
; Dieses Modul enth�lt nur Funktionsrahmen, die von Ihnen ausprogrammiert werden
; sollen.
*/


#include <hidef.h>                                      // Common defines
#include <mc9s12dp256.h>                                // CPU specific defines
#include <stdio.h>

#include "dcf77.h"
#include "led.h"
#include "clock.h"
#include "lcd.h"

#define secondsMinTime 900
#define secondsMaxTime 1100
#define minutesMinTime 1900
#define minutesMaxTime 2100
#define zeroMinTime 70
#define zeroMaxTime 130
#define oneMinTime 170
#define oneMaxTime 230

// Global variable holding the last DCF77 event
DCF77EVENT dcf77Event = NODCF77EVENT;

// Modul internal global variables
static int  dcf77Year=2017, dcf77Month=1, dcf77Day=1, dcf77Hour=0, dcf77Minute=0;       //dcf77 Date and time as integer values
char weekdayChar[3];
int lastdcfValue = 1;
int lastFallingEdge = 0;
char incomingSignal[59];
int signalPointer = 0;
int firstTime = 0;
// Prototypes of functions simulation DCF77 signals, when testing without
// a DCF77 radio signal receiver
void initializePortSim(void);                   // Use instead of initializePort() for testing
char readPortSim(void);                         // Use instead of readPort() for testing

// ****************************************************************************
// Initalize the hardware port on which the DCF77 signal is connected as input
// Parameter:   -
// Returns:     -
void initializePort(void)
{
    asm bclr DDRH, #1
}

// ****************************************************************************
// Read the hardware port on which the DCF77 signal is connected as input
// Parameter:   -
// Returns:     0 if signal is Low, >0 if signal is High
char readPort(void)
{
    char toReturn;       //initialize return variable
    asm{
        ldd #0          //load 0
        ldab PTH        //load pth
        andb #1         //so we can and them which leaves us with PTH[0]
        stab toReturn    //and we store that in our return variable
    }
    
    return toReturn;    //that we can return
}




// ****************************************************************************
//  Initialize DCF77 module
//  Called once before using the module
void initDCF77(void)
{   setClock((char) dcf77Hour, (char) dcf77Minute, 0);
    displayDateDcf77();
    initializePort();
}

// ****************************************************************************
// Display the date derived from the DCF77 signal on the LCD display, line 1
// Parameter:   -
// Returns:     -
void displayDateDcf77(void)
{   char datum[32];

    (void) sprintf(datum, "%s  %02d.%02d.%04d", weekdayChar, dcf77Day, dcf77Month, dcf77Year);
    writeLine(datum, 1);
}

// ****************************************************************************
//  Read and evaluate DCF77 signal and detect events
//  Must be called by user every 10ms
//  Parameter:  Current CPU time base in milliseconds
//  Returns:    DCF77 event, i.e. second pulse, 0 or 1 data bit or minute marker
DCF77EVENT sampleSignalDCF77(int currentTime)
{   
    int newVal = (int) readPort();                                   //first we read the Port !!!!!! IF ON HARDWARE; DONT USE readPortSim!!!!! USE readPort!!!!!!!!!!!
    int deltatime;
    
    if(newVal == 0) {
      setLED(0x02);  
    }else {
      clrLED(0x02);  
    }
              
    if(newVal == lastdcfValue) {                                        //if the new value is the same as the last
        return NODCF77EVENT;                                            //we return no event
    }    
    if(newVal < lastdcfValue) {                                         //if the new value is smaller than the last, we have a falling edge
        lastdcfValue = newVal;                                          //we then save the new val
        deltatime = currentTime - lastFallingEdge;                      //we then take the time difference between the current and the last edge
        lastFallingEdge = currentTime;                                  //and save it

        if(secondsMinTime <= deltatime && deltatime <= secondsMaxTime) {  //if the time difference is betwenn the seconds-time-bounds
            return VALIDSECOND;                                         //we have valid seconds event
        }
        if(minutesMinTime <= deltatime && deltatime <= minutesMaxTime) {  //if the time difference is betwenn the minutes-time-bounds
            return VALIDMINUTE;                                         //we have valid minutes event
        }
    }
    if(newVal > lastdcfValue) {                                         //if the new value is greater than the last, we have a rising edge
        lastdcfValue = newVal;                                          //we then save the new val
        deltatime = currentTime - lastFallingEdge;                      //we then take the time difference between the current and the last edge
        if(zeroMinTime <= deltatime && deltatime <= zeroMaxTime) {      //if the time difference is betwenn the zero-time-bounds
            return VALIDZERO;                                           //we have valid zero event
        }
        if(oneMinTime <= deltatime && deltatime <= oneMaxTime) {          //if the time difference is betwenn the one-time-bounds
            return VALIDONE;                                            //we have valid one event
        }
    }
    return INVALID;                                                     //if all fails, we have invalid event
}

void calculateValsFromSignal(void) {                                             
    int powers[] = {1,2,4,8,10,20,40,80};                                                               //we need a lookup for our decoding                                       
    int minutesNew = 0; int hoursNew = 0; int dayNew = 0; int monthNew = 0; int yearNew = 0;            //all values to change
    int paritycheck = 0;
    int loopcnt = 0;                                                                                    //all Loop counter
    int lengthMinutes = 7; int lengthHours = 6; int lengthDay = 6; int lengthWeekday = 3; int lengthMonth = 5; int lengthYear = 8;
    int weekday = 0;

    if(incomingSignal[20] != 1) {                                                                       //20st bit os always 1
        clrLED(0x08);
        return;
    }
    for(loopcnt; loopcnt< lengthMinutes; loopcnt++) {                                                   //now we check the bits 21-27 for the minutes
        paritycheck += (int) incomingSignal[loopcnt+21];                                                //we add the value of the bit to the paritycheck
        minutesNew += ((int) incomingSignal[loopcnt+21])*powers[loopcnt];                               //and add the value of the bit times 2^i to the minutes 
    }

    if(paritycheck % 2 != (int) incomingSignal[28]) {                                                   //now we can confirm the parity for the minutes via bit 28
        clrLED(0x08);
        return;                                                                                         
    }
    dcf77Minute = minutesNew;                                                                           //since the parity was fine, we can save the value
    paritycheck = 0;                                                                                    //reset our variables
    loopcnt = 0;
    for(loopcnt; loopcnt < lengthHours; loopcnt++) {                                                    //now we check the bits 29-34 for the minutes
        paritycheck += (int) incomingSignal[loopcnt+29];                                         //we add the value of the bit to the paritycheck
        hoursNew += ((int) incomingSignal[loopcnt+29]*powers[loopcnt]);                                 //and add the value of the bit times 2^i to the hours 
    }
    if(paritycheck % 2 != (int) incomingSignal[35]) {                                                   //now we can confirm the parity for the hours via bit 35
        return;
    }
    dcf77Hour = hoursNew;                                                                               //the hours are now correct
    setClock(dcf77Hour, dcf77Minute, '0');                                                                //we then set our clock with the new time and can assume seconds is 0 ##
    paritycheck = 0;                                                                                    //## since we are at the valid minute event
    loopcnt = 0;                                                                                        //reset our variables
    for(loopcnt; loopcnt < lengthDay; loopcnt++) {                                                      //now we check the bits 36-41 for the day
        paritycheck += (int) incomingSignal[loopcnt+36];                                         //we add the value of the bit to the paritycheck
        dayNew += ((int) incomingSignal[loopcnt+36]*powers[loopcnt]);                                   //and add the value of the bit times 2^i to the day 
    }

    loopcnt = 0;
    for(loopcnt; loopcnt < lengthWeekday; loopcnt++) {                                                  //now we check the bits 42-44 for the weekday
        paritycheck += (int) incomingSignal[loopcnt+42];                                         //we add the value of the bit to the paritycheck
        weekday += ((int) incomingSignal[loopcnt+42]*powers[loopcnt]);                                  //and add the value of the bit times 2^i to the weekday 
    }
    switch (weekday)                                                                                    //now we need to decide, what day it is
    {
    case 1:
        weekdayChar[0] = 'M'; weekdayChar[1] = 'o'; weekdayChar[2] = 'n';
        break;
    case 2:
        weekdayChar[0] = 'D'; weekdayChar[1] = 'i'; weekdayChar[2] = 'e';
        break;
    case 3:
        weekdayChar[0] = 'M'; weekdayChar[1] = 'i'; weekdayChar[2] = 't';
        break;
    case 4:
        weekdayChar[0] = 'D'; weekdayChar[1] = 'o'; weekdayChar[2] = 'n';
        break;
    case 5:
        weekdayChar[0] = 'F'; weekdayChar[1] = 'r'; weekdayChar[2] = 'e';
        break;
    case 6:
        weekdayChar[0] = 'S'; weekdayChar[1] = 'a'; weekdayChar[2] = 'm';
        break;
    case 7:
        weekdayChar[0] = 'S'; weekdayChar[1] = 'o'; weekdayChar[2] = 'n';
        break;    
    default:
        break;
    }
    loopcnt = 0;
    for(loopcnt; loopcnt < lengthMonth; loopcnt++) {                                                    //now we check the bits 45-49 for the day
        paritycheck += (int) incomingSignal[loopcnt+45];
        monthNew += ((int) incomingSignal[loopcnt+45]*powers[loopcnt]);
    }
    loopcnt = 0;
    for(loopcnt; loopcnt < lengthYear; loopcnt++) {                                                     //now we check the bits 50-57 for the day
        paritycheck += (int) incomingSignal[loopcnt+50];
        yearNew += ((int) incomingSignal[loopcnt+50]*powers[loopcnt]);
    }
    if(paritycheck % 2 != (int) incomingSignal[58]) {                                                   //now we can confirm the parity for the day, month and year via bit 58
        clrLED(0x08);
        return;
    }
    dcf77Day = dayNew; dcf77Month = monthNew; dcf77Year = yearNew+2000;                                 //we now have the new correct date
    setLED(0x08);
}

// ****************************************************************************
// Process the DCF77 events
// Contains the DCF77 state machine
// Parameter:   Result of sampleSignalDCF77 as parameter
// Returns:     -
void processEventsDCF77(DCF77EVENT event)
{
    switch (event)                                  //we start reading the signal, once we had our first valid minutes, so bit zero is at index zero of our incoming signal    
    {                                               //and we dont jump with the seconds
    case VALIDZERO:                                 //Valid zero -> we read a zero from the signal
        if(firstTime == 1) {                        //if we had our first valid minute
            incomingSignal[signalPointer] = 0;      //we write a zero at our pointer of our signal array
        }
        break;
    case VALIDONE:                                  //Valid one -> we read a one from the signal
        if(firstTime == 1) {                        //if we had our first valid minute
            incomingSignal[signalPointer] = 1;      //we write a one at our pointer of our signal array
        }
        break;
    case VALIDSECOND:                               //Valid second -> the last reading was fine
        if(firstTime == 1) {                        //if we had our first valid minute
            signalPointer++;                        //we step with our pointer
        }
        break;
    case VALIDMINUTE:
        
        if(firstTime == 0) {                        //if we hadn't our first valid minute
            firstTime = 1;                          //we now have
        }else {
            clrLED(0x04);                                     //if we had our first valid minute
            calculateValsFromSignal();                        //we step with our pointer
        }
                          //since we are now at second zero, we calculate the new values for the time and day
        
        signalPointer = 0;
        break;
    case INVALID:                                   //Invalid -> timing of signal was wrong
        //incomingSignal[signalPointer] = 'i';        //we mark it as invalid
        setLED(0x04);
        clrLED(0x08);
        break;
    default:
        break;
    }
                           //reset of signalpointer  
   
}
