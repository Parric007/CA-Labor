
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

; export symbols
        XDEF Entry, main

; import symbols
        XREF __SEG_END_SSTACK           ; End of stack

; include derivative specific macros
        INCLUDE 'mc9s12dp256.inc'

; Defines
                   

; RAM: Variable data section
.data: SECTION
counter: ds.w 1


; ROM: Constant data
.const: SECTION

; ROM: Code section
.init: SECTION

main:                                   ; Begin of the program
Entry:
        LDS  #__SEG_END_SSTACK          ; Initialize stack pointer
        CLI                             ; Enable interrupts, needed for debugger

        BSET DDRJ, #2
        BCLR PTJ, #2      
        MOVB #$FF, DDRB
        MOVB #$55, PORTB
        
        LDD #0000
        STD counter               
           
        loop:
            LDD counter      ;Load Counter from RAM
            INCB             ;Increment 2 Times
            INCB             ;
            CPD #64          ;At 64 Reset Counter via Overflow Branch
            BGE Overflow
            BRA Reset                  

Overflow:
       SUBD #63
       BRA Reset
        
Reset: STD counter          ;Store Counter in RAM
       STD PORTB            ;Output Counter to LED
       CLRB                 ;Reset D Register
       BRA loop             

SPEED:  EQU     $07CF
                   

delay_0_5sec:
     LDX #SPEED
     BRA Sleep               ;Otherwise return to Main Loop

Sleep:
     DBNE x, LoadX
     BRA loop               ;Decrement X Register, change to LoadX Branch if Zero                                       
      
SleepT:
     DBNE y, SleepT         ;Decrement Y Register, until Zero            
     BRA Sleep              ;Then return to outer Sleep Loop            
    
LoadX:
     LDY #SPEED             ;Reset Y Register and go to inner Sleep Loop             
     BRA SleepT           
      
      
      
      
      