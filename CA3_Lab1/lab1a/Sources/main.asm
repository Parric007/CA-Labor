
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
        
        INCLUDE 'delay.asm'
        INCLUDE 'led.asm'

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

        CALL init_LED
        LDD #0000
        STD counter               
           
        loop:
            LDAB counter      ;Load Counter from RAM
            INCB             ;Increment 2 Times
            INCB             ;
            CPD #64          ;At 64 Reset Counter via Overflow Branch
            BGE Overflow
            BRA Reset                  

Overflow:
       SUBD #63
       BRA Reset
        
Reset: STAB counter          ;Store Counter in RAM
       CALL set_LED          ;Output Counter to LED
       CLRB                  ;Reset D Register
       CLRA
       CALL delay_0_5sec     ;Sleep Loop gets called
       BRA loop             

      
      
      
      
      
      