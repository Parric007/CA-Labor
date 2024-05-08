
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

; Public interface function: checkButtons handles button input to alter the clock state
;
; Parameter: -
; Return:    -
; Registers: Unchanged (when function returns)
checkButtons:
      JSR setModeButton3
      JSR setModeButton4
      JSR setModeButton5
      
       
      BRCLR PTH, #$04, returnNoPress                 ;Check if Button 3 bit is set
      
      BRSET lastButtonStates, #$01, returnWithPress  ;if last state of button was on, return
      
      BSET lastButtonStates, #$01                    ;set last state of button to on
      
      PSHB
      LDAB #$10                                      ;change mode bit to the opposite
      EORB lastButtonStates                          ;technically we don't need the mode status bit at the moment but
      STAB lastButtonStates                          ;we keep track anyway 
      
      LDAB #$80
      EORB PORTB                                     ;the LED for set mode gets toggle accordingly to the mode we're in
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

      
setModeButton3:                                      ;Check if Button 4 bit is set
      BRCLR PTH, #$08, returnNoPress3
      BRSET lastButtonStates, #$02, returnWithPress
      BSET lastButtonStates, #$02
      
      JSR incHours                                   ;if all conditions are ok, we increment the hours
      
      RTS
 
 
      
 returnNoPress4:
      BCLR lastButtonStates, #$04
      RTS

      
setModeButton4:
      BRCLR PTH, #$10, returnNoPress4                ;Check if Button 5 bit is set
      BRSET lastButtonStates, #$04, returnWithPress
      BSET lastButtonStates, #$04
      
      JSR incMinutes                                 ;if all conditions are ok, we increment the minutes
      
      RTS
 
 
      
 returnNoPress5:
      BCLR lastButtonStates, #$08
      RTS

      
setModeButton5:
      BRCLR PTH, #$20, returnNoPress5                ;Check if Button 6 bit is set
      BRSET lastButtonStates, #$08, returnWithPress
      BSET lastButtonStates, #$08
      
      JSR incrementTime                              ;if all conditions are ok, we increment the seconds
      
      RTS     
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
             
