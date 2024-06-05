/*  Radio signal clock - Free running clock

    Computerarchitektur 3
    (C) 2018 J. Friedrich, W. Zimmermann Hochschule Esslingen

    Author:   W.Zimmermann, Jun  10, 2016
    Modified: -
*/

#include <stdio.h>
#include <mc9s12dp256.h>
#include "clock.h"
#include "lcd.h"
#include "led.h"
#include "dcf77.h"

// Defines
#define ONESEC  (1000/10)                       // 10ms ticks per second
#define MSEC200 (200/10)
#define MASK_H_3 0x08

// Global variable holding the last clock event
CLOCKEVENT clockEvent = NOCLOCKEVENT;

// Modul internal global variables
static char hrs = 0, mins = 0, secs = 0;
static int uptime = 0;
static int ticks = 0;
static unsigned char lastButtonState = 0;
static unsigned char buttonWaiting = 0;
char timezone[3];
int offset = 0;

// ****************************************************************************
//  Initialize clock module
//  Called once before using the module
void initClock(void)
{   displayTimeClock();
    DDRH &= 0xFB;
    timezone[0] = 'D';
    timezone[1] = 'E';
    timezone[2] = '\0';
}

void changeOffset(void) {
    if(offset == 0){
        offset = 6;
        timezone[0] = 'U';
        timezone[1] = 'S';
        timezone[2] = '\0';
        hrs -= 6;
        if(hrs<0) {
            hrs += 24;
            decDay();
        }
        return;  
    }
    offset = 0;
    hrs += 6;
    if(hrs > 23) {
        hrs -= 24;
        incDay();
    }
    timezone[0] = 'D';
    timezone[1] = 'E';
    timezone[2] = '\0';
}

// ****************************************************************************
// This function is called periodically every 10ms by the ticker interrupt.
// Keep processing short in this function, run time must not exceed 10ms!
// Callback function, never called by user directly.
void tick10ms(void)
{   
    unsigned char button = ~PTH;
    unsigned char detected = lastButtonState ^ button;
    if (++ticks >= ONESEC)                      // Check if one second has elapsed
    {   clockEvent = SECONDTICK;                // ... if yes, set clock event
        ticks=0;
        setLED(0x01);                           // ... and turn on LED on port B.0 for 200msec
    } else if (ticks == MSEC200)
    {   clrLED(0x01);
    }
    uptime = uptime + 10;                       // Update CPU time base

    dcf77Event = sampleSignalDCF77(uptime);     // Sample the DCF77 signal
    
    //--- Add code here, which shall be executed every
    
    
   
    if((detected & MASK_H_3) & ~(lastButtonState & MASK_H_3)) {
          buttonWaiting = buttonWaiting | MASK_H_3;
    }
    lastButtonState = button;
    
    if(buttonWaiting & MASK_H_3) {
        changeOffset();  
    }
    
    buttonWaiting = buttonWaiting & ~MASK_H_3;
    //--- End of user code
}



// ****************************************************************************
// Process the clock events
// This function is called every second and will update the internal time values.
// Parameter:   clock event, normally SECONDTICK
// Returns:     -
void processEventsClock(CLOCKEVENT event)
{   if (event==NOCLOCKEVENT)
        return;

    if (++secs >= 60)
    {   secs = 0;
        if (++mins >= 60)
        {   mins = 0;
            if (++hrs >= 24)
            {   hrs = 0;
            }
        }
     }
}

// ****************************************************************************
// Allow other modules, e.g. DCF77, so set the time
// Parameters:  hours, minutes, seconds as integers
// Returns:     -
void setClock(char hours, char minutes, char seconds)
{   hrs  = hours;
    mins = minutes;
    secs = seconds;
    ticks = 0;
}

// ****************************************************************************
// Display the time derived from the clock module on the LCD display, line 0
// Parameter:   -
// Returns:     -
void displayTimeClock(void)
{   char uhrzeit[32] = "00:00:00";
    (void) sprintf(uhrzeit, "%02d:%02d:%02d %s", hrs, mins, secs, timezone );
    writeLine(uhrzeit, 0);
}

// ***************************************************************************
// This function is called to get the CPU time base
// Parameters:  -
// Returns:     CPU time base in milliseconds
int time(void)
{   return uptime;
}
