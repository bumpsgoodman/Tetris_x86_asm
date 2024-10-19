.code

;--------------------------------------
; num - 문자로 바꿀 숫자
; pOutBytes - 문자로 변환된 배열
; 반환값 - 변환된 문자 개수
;
; 0 <= num < 1000
;--------------------------------------
IntToAscii PROC num:DWORD, pOutBytes:PTR BYTE
    ; eax - num
    ; ecx - 변환된 자릿수

    push edi            ; edi 백업
    push ebx            ; ebx 백업

    mov edi, pOutBytes

    mov eax, num
    mov ebx, 10

    ; num이 0인 경우 '0' 출력
    test eax, eax
    jnz lb_hundreds_digit

    mov BYTE PTR [edi], '0'
    mov ecx, 1
    jmp lb_return

lb_hundreds_digit:
    ; 100의 자릿수인지
    cmp eax, 100
    jl lb_tens_digit
    mov ecx, 3
    add edi, 2

    jmp lb_loop

lb_tens_digit:
    ; 10의 자릿수인지
    cmp eax, 10
    jl lb_unit_digit
    mov ecx, 2

    inc edi

    jmp lb_loop

lb_unit_digit:
    mov ecx, 1

lb_loop:
    ; 몫이 0이면 종료
    test eax, eax
    je lb_return

    xor edx, edx
    div ebx

    ; 숫자 -> 문자로 변환 후 저장
    add dl, '0'
    mov BYTE PTR [edi], dl
    dec edi

    jmp lb_loop

lb_return:
    pop ebx
    pop edi
    mov eax, ecx
    ret
    
IntToAscii ENDP