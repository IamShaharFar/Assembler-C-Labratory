START:  mov r3 LENGTH     ; ❌ Missing comma
        sub ,r1           ; ❌ Consecutive comma
        add #abc, r2      ; ❌ Invalid immediate
        prn               ; ❌ Missing parameter
        stop extra        ; ❌ Extra text
        jmp &UNDEF_LABEL  ; ❌ Undefined label
