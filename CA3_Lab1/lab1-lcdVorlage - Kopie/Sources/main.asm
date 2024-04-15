;
;   Labor 1 - Test program for LCD driver
;
;   Computerarchitektur
;   (C) 2019-2022 J. Friedrich, W. Zimmermann, R. Keller
;   Hochschule Esslingen
;
;   Author:   	   J.Friedrich, W. Zimmermann
;   Last Modified: R. Keller, August 2022
; Export symbols
        XDEF Entry, main

; Import symbols
        XREF __SEG_END_SSTACK                   ; End of stack
        XREF initLCD, writeLine, delay_10ms     ; LCD functions
        XREF init_LED, set_LED, get_LED, toggle_LED       ;LED functions
        XREF delay_0_5sec                       ;delay
        XREF hexToASCII, decToASCII             ;conversions
        

; Include derivative specific macros
        INCLUDE 'mc9s12dp256.inc'

; Defines

; RAM: Variable data section
.data:  SECTION
locDec:  ds.b 8
locHex:  ds.b 8
counter: dc.w 1


; ROM: Constant data
.const: SECTION
MSG1:   dc.b " Mach mal eine ",0
MSG2:   dc.b " kleine Pause  ", 0
msgA: DC.B "ABCDEFGHIJKLMnopqrstuvwxyz1234567890", 0
msgB: DC.B "is this OK?", 0 
msgC: DC.B "Keep texts short!", 0 
msgD: DC.B "Oh yeah!", 0 


; ROM: Code section
.init:  SECTION

main:
Entry:
        
        LDS  #__SEG_END_SSTACK          ; Initialize stack pointer
        CLI                             ; Enable interrupts, needed for debugger

        JSR  delay_10ms                 ; Delay 20ms during power up
        JSR  delay_10ms                 ; by Jump-Subroutine (use step-over)

        JSR  initLCD                    ; Initialize the LCD
        JSR  init_LED
           
        
        MOVW #0, counter
      
loop:   
        LDD counter                          
        LDX #locDec
        JSR decToASCII
        LDAB #1
        JSR writeLine
        
        LDD counter
        LDX #locHex
        JSR hexToASCII
        LDAB #0
        JSR writeLine
        
        LDD counter
        JSR set_LED
        
        
        
        JSR delay_0_5sec
        LDD counter
        Addd #1
        STD counter
        
        BRCLR PTH, #$01, button0pressed ; check if button on port PTH.0
        BRCLR PTH, #$02, button1pressed
        BRCLR PTH, #$04, button2pressed
        BRCLR PTH, #$08, button3pressed
        
        bra loop
        
        

back:   BRA back


button0pressed:
        LDD counter
        addd #15
        STD counter
        BRA loop
        
        
button1pressed:
         LDD counter
         addd #9
         std counter 
         BRA loop
         
          
button2pressed:
         LDD counter
         subd #17
         std counter 
         BRA loop
         
         
button3pressed:        
         LDD counter
         subd #11
         std counter 
         BRA loop
         

reset:
        MOVW #0, counter
        BRA loop