.extern EXT_LABEL
.entry START
.entry CALC

START:  mov r3, LENGTH
        add r2, r3
        sub #5, r1
        lea STR, r4
        prn r4
        jsr &CALC
        jmp EXT_LABEL
        stop

LENGTH: .data 6
STR:    .string "Hello"
CALC:   inc r2
        dec r1
        rts

