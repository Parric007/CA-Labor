;   Labor 1 - Vorbereitungsaufgabe 2.4
;   Convert a zero-terminated ASCIIZ string to lower characters
;   Main program
;
;   Computerarchitektur
;   (C) 2019-2022 J. Friedrich, W. Zimmermann, R. Keller
;   Hochschule Esslingen
;
;   Author:   R. Keller, Jul 4, 2019
;            (based on code provided by J. Friedrich, W. Zimmermann)
;   Modified: -
;

; export symbols
        XDEF Entry, main

; import symbols
        XREF __SEG_END_SSTACK           ; End of stack
        XREF toLower                    ; Referenced from other object file
        XREF strCpy
        XREF hexToASCII
        XREF decToASCII

; include derivative specific macros
        INCLUDE 'mc9s12dp256.inc'

; Defines

; RAM: Variable data section
.data:  SECTION
Vtext:  DS.B    80                      ; Please store String here
Htext:  DS.B    7
Dtext:  DS.B    7

; ROM: Constant data
.const: SECTION
Ctext:  DC.B  "Test 12345 *!? ABCDE abcde zZ", 0

; ROM: Code section
.init:  SECTION

main:                                   ; Begin of the program
Entry:
        LDS  #__SEG_END_SSTACK          ; Initialize stack pointer
        CLI                             ; Enable interrupts, needed for debugger

;       ... ??? ...                     ; Add your code here
;
;  To Do
;       Copy string from Ctext in ROM to Vtext in RAM
;       Load pointer to string in D
;       Call subroutine toLower

        
        LDX #Ctext
        LDY #Vtext
        
        BSR strCpy
        
        LDD #Vtext
        BSR toLower
        
        LDD #$FFFF
        LDX #Htext
        BSR hexToASCII
        
        LDD #32767   ; D: FFFF
        LDX #Dtext
        BSR decToASCII
        
        
        
                
loop:
        BRA loop