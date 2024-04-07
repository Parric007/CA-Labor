   
   
   
        INCLUDE 'mc9s12dp256.inc'

.init SECTION

init_LED:
      BSET DDRJ, #2
      BCLR PTJ, #2      
      MOVB #$FF, DDRB
      MOVB #$FF, PORTB
      RTC

set_LED:
      STAB PORTB
      RTC
      
get_LED:
      LDAB PORTB
      RTC

toggle_LED:
      EORB PORTB
      STAB PORTB
      RTC
              