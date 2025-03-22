.entry START
.extern EXT_FUNC

;comments to be ignored
 
START:  mov r1, LENGTH ;also comments here
        jsr EXT_FUNC
        prn r1
        stop

LENGTH: .data 10, -2
STR: .string "str"
