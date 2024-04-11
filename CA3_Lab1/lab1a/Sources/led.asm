   
  XDEF init_LED
  XDEF set_LED
  XDEF get_LED
  XDEF toggle_LED  
  INCLUDE 'mc9s12dp256.inc'
  

.init SECTION

init_LED:
      BSET DDRJ, #2                  
      BCLR PTJ, #2      
      MOVB #$FF, DDRB
      MOVB #$FF, PORTB
      RTS

set_LED:
      NEGB                           ;we negate b register
      STAB PORTB                     ;and store it at our ledport
      RTS
      
get_LED:
      LDAB PORTB                     ;we load our ledport into b register
      RTS

toggle_LED:
      EORB PORTB                     ;we perform the exclusive-or-operation
      STAB PORTB                     ;and store the result in our ledport
      RTS
              