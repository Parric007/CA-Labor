


      INCLUDE 'mc9s12dp256.inc'
      
      XDEF hexToASCII
      XDEF decToASCII


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
        PSHX
        PSHD
        PSHY
        
        CMPA #0
        BLT addMinusSign
        MOVB #$20 , X
        INX                             


convertDecimal:
        TFR x, y
        LDX #10000  
        IDIV
        BSET x, #$30
        STX y
        INY
        LDX #1000  
        IDIV
        BSET x, #$30
        STX y
        INY
        LDX #100  
        IDIV
        BSET x, #$30
        STX y
        INY
        LDX #10  
        IDIV
        BSET x, #$30
        STX y
        INY
        TFR d, x
        BSET x, #$30
        STX y
        INY
        MOVB #0, y
        PULY
        PULD
        PULX
        RTS        

        
addMinusSign:
        MOVB #$2D , X
        INX
        ADDD #$8000
        BRA convertDecimal 
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        