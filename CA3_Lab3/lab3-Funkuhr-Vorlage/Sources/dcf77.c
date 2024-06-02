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

    (void) sprintf(datum, "%02d.%02d.%04d", dcf77Day, dcf77Month, dcf77Year);
    writeLine(datum, 1);
}

// ****************************************************************************
//  Read and evaluate DCF77 signal and detect events
//  Must be called by user every 10ms
//  Parameter:  Current CPU time base in milliseconds
//  Returns:    DCF77 event, i.e. second pulse, 0 or 1 data bit or minute marker
DCF77EVENT sampleSignalDCF77(int currentTime)
{   
    int newVal = (int) readPortSim();                                      //first we read the Port
    int deltatime;          
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
        if(zeroMinTime <= deltatime && deltatime <= zeroMaxTime) {        //if the time difference is betwenn the zero-time-bounds
            return VALIDZERO;                                           //we have valid zero event
        }
        if(oneMinTime <= deltatime && deltatime <= oneMaxTime) {          //if the time difference is betwenn the one-time-bounds
            return VALIDONE;                                            //we have valid one event
        }
    }
    return INVALID;                                                     //if all fails, we have invalid event
}

void calculateValsFromSignal(void) {                                             
    int powers[] = {1,2,4,8,10,20,40,80};                               //we need a lookup for our decoding                                       
    int minutesNew = 0; int hoursNew = 0; int dayNew = 0; int monthNew = 0; int yearNew = 0;            //all values to change
    int paritycheck = 0;
    int loopcnt = 0;                                           //all Loop counter
    int lengthMinutes = 7; int lengthHours = 6; int lengthDay = 6; int lengthWeekday = 3; int lengthMonth = 5; int lengthYear = 8;


    if(incomingSignal[20] != 1) {
        return;
    }
    for(loopcnt; loopcnt< lengthMinutes; loopcnt++) {
        paritycheck += (int) incomingSignal[loopcnt+21];
        minutesNew += ((int) incomingSignal[loopcnt+21])*powers[loopcnt];
    }

    if(paritycheck % 2 != (int) incomingSignal[28]) {
        return;
    }
    dcf77Minute = minutesNew;
    paritycheck = 0;
    loopcnt = 0;
    for(loopcnt; loopcnt < lengthHours; loopcnt++) {
        paritycheck = paritycheck & incomingSignal[loopcnt+29];
        hoursNew += ((int) incomingSignal[loopcnt+29]*powers[loopcnt]);
    }
    if(paritycheck % 2 != (int) incomingSignal[35]) {
        return;
    }
    dcf77Hour = hoursNew;
    setClock(dcf77Hour, dcf77Minute, 0);
    paritycheck = 0;
    loopcnt = 0;
    for(loopcnt; loopcnt < lengthDay; loopcnt++) {
        paritycheck = paritycheck & incomingSignal[loopcnt+36];
        dayNew += ((int) incomingSignal[loopcnt+36]*powers[loopcnt]);
    }
    loopcnt = 0;
    for(loopcnt; loopcnt < lengthMonth; loopcnt++) {
        paritycheck = paritycheck & incomingSignal[loopcnt+45];
        monthNew += ((int) incomingSignal[loopcnt+45]*powers[loopcnt]);
    }
    loopcnt = 0;
    for(loopcnt; loopcnt < lengthYear; loopcnt++) {
        paritycheck = paritycheck & incomingSignal[loopcnt+50];
        yearNew += ((int) incomingSignal[loopcnt+50]*powers[loopcnt]);
    }
    if(paritycheck % 2 != (int) incomingSignal[58]) {
        return;
    }
    dcf77Day = dayNew; dcf77Month = monthNew; dcf77Year = yearNew+2000;
}

// ****************************************************************************
// Process the DCF77 events
// Contains the DCF77 state machine
// Parameter:   Result of sampleSignalDCF77 as parameter
// Returns:     -
void processEventsDCF77(DCF77EVENT event)
{
    switch (event)
    {
    case VALIDZERO:
        if(firstTime == 1) {
            incomingSignal[signalPointer] = 0;
        }
        break;
    case VALIDONE:
        if(firstTime == 1) {
            incomingSignal[signalPointer] = 1;
        }
        break;
    case VALIDSECOND:
        if(firstTime == 1) {
            signalPointer++;
        }
        break;
    case VALIDMINUTE:
        
        if(firstTime == 0) {
            firstTime = 1;
        }else {
            signalPointer++;
        }
        calculateValsFromSignal();
        break;
    case INVALID:
        incomingSignal[signalPointer] = 'i';
        break;
    default:
        break;
    }
    if(signalPointer == 59) {
        signalPointer = 0;  
    }
    
}