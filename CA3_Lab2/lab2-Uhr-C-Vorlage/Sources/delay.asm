                               
;   Labor 1 - Problem 2.2
;   Incrementing a value once per second and binary output to LEDs
;
;   Computerarchitektur
;   (C) 2019-2022 J. Friedrich, W. Zimmermann, R. Keller
;   Hochschule Esslingen
;
;   Author:   R. Keller, HS-Esslingen
;            (based on code provided by J. Friedrich, W. Zimmermann)
;   Modified: -
;


      XDEF delay_0_5sec


.init SECTION
; Defines
SPEED:  EQU     $07CF
                   

delay_0_5sec:
     LDX #SPEED
     BRA Sleep               ;Otherwise return to Main Loop

Sleep:
     DBNE x, LoadX
     RTS                    ;Decrement X Register, change to LoadX Branch if Zero                                       
      
SleepT:
     DBNE y, SleepT         ;Decrement Y Register, until Zero            
     BRA Sleep              ;Then return to outer Sleep Loop            
     
LoadX:
     LDY #SPEED             ;Reset Y Register and go to inner Sleep Loop             
     BRA SleepT                          