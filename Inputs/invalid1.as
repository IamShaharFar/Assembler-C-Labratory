start: mov r1, #9      ;  second operand cant be #
add r3, r2,           ; extra text
sub ,r4, r5           ; comma before operands
jmp start extra       ; unexpected extra text
1label: add r2, r3      ; labels can't start with a digit
validLabel add r1, r2   ; unrecognized command
.data 100,200,          ; trailing comma after last number
.string "Hello         ; missing closing quote
.extern validLabel      ; label cannot be both external and local
