


      INCLUDE 'mc9s12dp256.inc'
      
      XDEF hexToASCII
      XDEF decToASCII


.data: SECTION

.const: SECTION
lookup: DC.b "0123456789ABCDEF"

.init: SECTION      


      
hexToASCII:                          ;Location to Store: X  value to store: D
        PSHD
        PSHY                         ;first we save our registers to the stack
        PSHX                         ;so we can restore them later
        MOVB #$30, X                 ;since we write hex Values into memory we need to
        INX                          ;start with 0, increment our memory pointer
        MOVB #$78, X                 ;write x into memory
        INX                          ;and increment our memory pointer once again

        LDY #4                       ;now we load a 4 into y register so we have a loop counter
        
       

convertLoop:
        PSHA                         ;we push a register for later
        BSR getASCII                 ;branch into the subroutine getAscii
        STAA X                       ;after getASCII the Ascii value is in a which we store in memory
        INX                          ;we increment our memory pointer
        PULA                         ;restore our a register
        LSLD                         ;now we need to perform 4 logic shift left d
        LSLD                         ;so we get the next nibble to the left end of the d register
        LSLD
        LSLD
        DBNE y, convertLoop          ;if our loop counter is not zero we need to redo the process above
        MOVB #$00, X                 ;if we did all 4 nibbles we write the terminating zero
        PULX                         ;and the final step is to restore our registers
        PULY                         ;to their previous state from the stack
        PULD
        
        RTS                          ;now we can return our subroutine
        
getASCII:                            ;loop counter needs to be saved
        PSHY                         ;we perform the and-operation so we get the first nibble
        ANDA #$F0                    ;now we need to shift our register 4 times
        LSRA                         ;so the nibble is on the right end of the register
        LSRA
        LSRA
        LSRA
        TFR a, y                     ;this value we transfer into y register
        LDAA lookup, y               ;so we can use it as offset in our lookup table
        PULY                         ;we restore our loop counter
        RTS                          ;and return back to the convertLoop
  
        
        
decToASCII:
        PSHX                         ;first we save our registers on the stack
        PSHD
        PSHY
        
        CMPA #0                      ;If a is smaller than 0 we branch
        BLT addMinusSign             ;into the addMinusSign method
        MOVB #$20 , X                ;Otherwise we write a space into memory
        INX                          ;and increment our memory pointer   


convertDecimal:
        TFR x, y                     ;first step is to get our memory pointer into y register
        
        LDX #10000                   ;we load 10.000 into d register
        IDIV                         ;idiv writes the result into x register and remainder in d
        PSHB                         ;we push our b register so we can
        LDAB #$30                    ;load $30 into it so lastly we can
        ABX                          ;add b to x so we have the ascii value in our x register
        TFR x, b                     ;this value we transfer back to b
        STAB y                       ;so we can store 2 bytes at our memory pointer
        PULB                         ;now we just need to restore b from the stack
        INY                          ;increment our memory pointer
        
        LDX #1000                    ;This process we repeat with 1000
        IDIV
        PSHB
        LDAB #$30
        ABX
        TFR x, b
        STAB y
        PULB
        INY
        
        LDX #100                     ;and again with 100
        IDIV
        PSHB
        LDAB #$30
        ABX
        TFR x, b
        STAB y
        PULB
        INY
        
        LDX #10                      ;and one last time with 10
        IDIV
        PSHB
        LDAB #$30
        ABX
        TFR x, b
        STAB y
        PULB
        INY
        
        TFR d, x                     ;so now all that is left is the remainder in d register
        LDAB #$30                    ;which we again increment
        ABX                          ;with $30 so we have the ascii value
        TFR x, b                     ;and that we can write once again into memory
        STAB y                       ;at the address of our memory pointer
        
        INY                          ;now we need to increment the memory pointer once again
        MOVB #0, y                   ;and write the terminating zero
        
        PULY                         ;the final step is to restore
        PULD                         ;all registers to their previous state
        PULX                         ;before the function call
        RTS                          ;and return our subroutine

        
addMinusSign:
        MOVB #$2D , X                ;first we write a minus sign into memory
        INX                          ;and increment our memory pointer
        COMA                         ;then we get the complement of our D register
        COMB                         ;seperate a and b because d is lazy
        ADDD #1                      ;then we add 1 and then we have the unsigned value of input
        BRA convertDecimal           ;after that we jump to the convert method
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        