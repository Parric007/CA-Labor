


      INCLUDE 'mc9s12dp256.inc'
      
      XDEF hexToASCII


.data: SECTION

.const: SECTION
lookup: DC.b "0123456789ABCDEF"

.init: SECTION      
      
hexToASCII:                          ;Location to Store: X  value to store: D
        PSHD
        PSHY
        PSHX
        MOVB #$30, X
        INX
        MOVB #$78, X
        INX

        
        LDY #4
        
       

convertLoop:
        PSHA
        BSR getASCII
        STAA X
        INX
        PULA
        LSLD
        LSLD
        LSLD
        LSLD
        DBNE y, convertLoop 
        MOVB #$00, X 
        PULX
        PULY
        PULD
        
        RTS                
        
getASCII:
        PSHY
        ANDA #$F0
        LSRA
        LSRA
        LSRA
        LSRA
        TFR a, y
        LDAA lookup, y
        PULY
        RTS       
  
        
        
decToASCII:
        
        