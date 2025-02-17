mcro MAC1
    mov r1, r2
    add r3, r4
mcroend
MAC1
.extern EXTERN_LABEL
.entry ENTRY_LABEL

LABEL1: .data 5, 10, 15
LABEL2: .data -3, 7, 12
LABEL3: .string "Hello, World!" 
LABEL4: .string "Valid String" 
MAC1
mov r1, r2
cmp #5, r3
add r1, r4
sub r2, r5
lea ENTRY_LABEL, r6

inc r3
dec r5
clr r7

jmp EXTERN_LABEL
bne ENTRY_LABEL
jsr ENTRY_LABEL

prn r1
prn #42

stop
rts

mcro MAC2
    prn #99
    clr r3
mcroend
MAC2
MAC2