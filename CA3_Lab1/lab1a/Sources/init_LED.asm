   
   
   
        INCLUDE 'mc9s12dp256.inc'

.init SECTION

init_LED:
      BSET DDRJ, #2
      BCLR PTJ, #2      
      MOVB #$FF, DDRB
      MOVB #$55, PORTB
        