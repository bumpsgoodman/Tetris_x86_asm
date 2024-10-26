.code

EXTRN GetTickCount@0 : PROC

;=========================================
; 함수 이름: GetRandomRange
; 설명: 이 함수는 지정된 범위 내에서의 랜덤 값을 반환합니다.
; 매개변수:
;   - range (ebp + 8)
; 반환값: 32비트 랜덤 숫자 (최대값: range - 1)
;=========================================
GetRandomRange:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    ; seed 설정
    call GetTickCount@0

    ; Xorshift 랜덤 생성
    ; x ^= x << 13
    mov ebx, eax
    shl ebx, 13
    xor eax, ebx

    ; x ^= x >> 17
    mov ebx, eax
    shr ebx, 17
    xor eax, ebx

    ; x ^= x << 5
    mov ebx, eax
    shl ebx, 5
    xor eax, ebx

    ; eax = eax % range
    xor edx, edx
    div [ebp + 8]
    mov eax, edx

lb_get_random_ragne_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret