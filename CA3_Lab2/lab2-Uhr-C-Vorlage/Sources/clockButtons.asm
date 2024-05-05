                                                    
        XDEF checkButtons
        
        XREF incHours
        XREF incMinutes
        XREF incrementTime       

; Include derivative specific macros
        INCLUDE 'mc9s12dp256.inc'
        
        

; RAM: Variable data section
.data:  SECTION
lastButtonStates DC.B 0                  ;bits 3 downto 0 represent buttons 5-2,  bit 4 1 = setmode


; ROM: Constant data
.const: SECTION             ;1,2,4,8 States

                            ;2,3,4,5, Buttons
; ROM: Code section
.init:  SECTION             ;04,08,10,20 Ports


checkButtons:
      JSR setModeButton3
      JSR setModeButton4
      JSR setModeButton5
      
       
      BRCLR PTH, #$04, returnNoPress      ;BRCLR
      
      BRSET lastButtonStates, #$01, returnWithPress
      
      BSET lastButtonStates, #$01
      
      PSHB
      LDAB #$10
      EORB lastButtonStates
      STAB lastButtonStates
      
      LDAB #$80
      EORB PORTB
      STAB PORTB     
      PULB 

      RTS
           
returnWithPress:
      RTS

returnNoPress:
      BCLR lastButtonStates, #$01
      RTS



returnNoPress3:
      BCLR lastButtonStates, #$02
      RTS

      
setModeButton3:
      BRCLR PTH, #$08, returnNoPress3
      BRSET lastButtonStates, #$02, returnWithPress
      BSET lastButtonStates, #$02
      
      JSR incHours
      
      RTS
 
 
      
 returnNoPress4:
      BCLR lastButtonStates, #$04
      RTS

      
setModeButton4:
      BRCLR PTH, #$10, returnNoPress4
      BRSET lastButtonStates, #$04, returnWithPress
      BSET lastButtonStates, #$04
      
      JSR incMinutes
      
      RTS
 
 
      
 returnNoPress5:
      BCLR lastButtonStates, #$08
      RTS

      
setModeButton5:
      BRCLR PTH, #$20, returnNoPress5
      BRSET lastButtonStates, #$08, returnWithPress
      BSET lastButtonStates, #$08
      
      JSR incrementTime
      
      RTS     