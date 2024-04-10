;   Labor 1 - Problem 2.4
;   Convert a zero-terminated ASCIIZ string to lower characters
;   Subroutine toLower
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
        XDEF toLower
        XDEF strCpy

; Defines

; RAM: Variable data section
.data: SECTION

; ROM: Constant data
.const: SECTION

; ROM: Code section
.init: SECTION

toLower:
        PSHX
        PSHA
        TFR d,x

lowerloop:
        LDAA x                       ;Load Value in A
        CMPA #$41                    ;Compare if Ascii Value
        BLO skipToLower              ;is between 41 and 5a
        CMPA #$5A                    ;which represents uppercase
        BHI skipToLower              ;letters
                                     
        ADDA #32                     ;if thats the case increment the value 
        STAA x                       ;by 32 and store it back in its location
        
skipToLower:                               
        INX                              
        TBNE a, lowerloop
        PULA
        PULX
        RTS
        
        
strCpy:
        PSHD
        LDAB #0                      ;Startoffset = 0
        
copyloop:
        LDAA b, x                    ;Letter to Copy -> A
        STAA b, y                    ;Store Letter in RAM
        INCB                         ;Increment offset
        TBNE a, copyloop             ;If Letter != 0 repeat
        PULD
        RTS                 

endloop:
        BRA endloop
      

