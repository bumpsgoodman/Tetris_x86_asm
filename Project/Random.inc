.code

EXTRN GetTickCount@0 : PROC

; 반환값: 32비트 랜덤 숫자
GetRandom PROC
    ; seed 설정
    call GetTickCount@0

    ; Xorshift 랜덤 생성
    ; x ^= x << 13
    mov ecx, eax
    shl ecx, 13
    xor eax, ecx

    ; x ^= x >> 17
    mov ecx, eax
    shr ecx, 17
    xor eax, ecx

    ; x ^= x << 5
    mov ecx, eax
    shl ecx, 5
    xor eax, ecx

    ret
GetRandom ENDP

; 반환값: 32비트 랜덤 숫자 (최대값: range - 1)
GetRandomRange PROC range:DWORD
    ; seed 설정
    call GetTickCount@0

    ; Xorshift 랜덤 생성
    ; x ^= x << 13
    mov ecx, eax
    shl ecx, 13
    xor eax, ecx

    ; x ^= x >> 17
    mov ecx, eax
    shr ecx, 17
    xor eax, ecx

    ; x ^= x << 5
    mov ecx, eax
    shl ecx, 5
    xor eax, ecx

    ; eax = eax % range
    xor edx, edx
    div range
    mov eax, edx

    ret
GetRandomRange ENDP