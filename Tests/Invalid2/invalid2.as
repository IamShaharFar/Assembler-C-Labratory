.entry MISSING_LABEL      ; ❌ undefined entry label
LABEL:  lea #5, r1        ; ❌ invalid source operand for lea
        jsr &EXT_LABEL    ; ❌ relative addressing on external label
.extern EXT_LABEL
        add r3 r2         ; ❌ missing comma
        mov r1, ,r3       ; ❌ consecutive commas
        stop unknown      ; ❌ extra text after command
