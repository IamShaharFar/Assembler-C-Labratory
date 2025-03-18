mcro mov ;reserved word
mcroend
start: add r1, #256       ; immediate value out of range
mov r3, r2, r4           ; too many operands
cmp "string", r2         ; invalid operand type
lea 100, r1              ; first operand must be a label, not a number
invalid label: mov r1, r2 ; labels cannot contain spaces
.extern externalLabel, anotherLabel  ; extern directive cannot have multiple labels
.entry externalLabel       ; entry label must be previously defined in the file
.extern nowExtern
.entry nowExtern    ;entry cannot be external