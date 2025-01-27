.code

;=========================================
; 함수 이름: IntegerToAscii
; 설명: 이 함수는 정수를 아스키 문자로 변환해줍니다.
; 매개변수:
;   - 첫 번째 매개변수(num): 변환할 정수 숫자 (ebp + 8)
;   - 두 번째 매개변수(pOutBytes): 아스키 문자로 변환된 숫자를 저장할 포인터 (ebp + 12)
; 반환값: 아스키 문자로 변환된 숫자 개수
;
; num에 범위는 다음과 같습니다.
; 0 <= num < 1000
;=========================================
IntegerToAscii:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edi

    mov ebx, [ebp + 8]  ; ebx = num
    mov edi, [ebp + 12] ; edi = pOutBytes

    ; num == 0인 경우,
    ; '0' 저장 후, 종료
    test ebx, ebx
    jnz lb_test_digit3

    mov al, '0'
    mov [edi], al  ; pOutBytes[0] = '0'

    ; 리턴
    xor eax, eax
    jmp lb_integer_to_ascii_return

lb_test_digit3:
    mov edx, 3  ; 먼저, 세 자릿수로 가정

    ; 세 자릿수인지 판단
    cmp ebx, 100
    jge lb_init_convert

    dec edx ; 자릿수 감소, 두 자릿수

    ; 두 자릿수인지 판단
    cmp ebx, 10
    jge lb_init_convert

    dec edx ; 자릿수 감소, 한 자릿수

lb_init_convert:
    add edi, edx    ; 뒷자리부터 채워 나가야 하기 때문에, 자릿수 - 1만큼 포인터 증가
    dec edi
    mov ecx, edx    ; 루프 카운트 설정
    mov ax, bx      ; 초기값(num)을 ax에 저장
    mov bl, 10      ; 10으로 나눌 것임

lb_convert:
    div bl

    add ah, '0'     ; 숫자 -> 아스키 문자
    mov [edi], ah   ; 아스키 문자 저장
    dec edi

    xor ah, ah      ; 나머지가 ah에 저장되기 때문에 다음 연산에 문제가 생길 수 있어, ah를 0으로 초기화
    loop lb_convert

    mov eax, edx

lb_integer_to_ascii_return:
    ; 에필로그
    pop edi
    pop ebx
    mov esp, ebp
    pop ebp
    ret 8

;=========================================
; 함수 이름: IntegerToAscii2
; 설명: 이 함수는 정수를 아스키 문자로 변환해줍니다.
; 매개변수:
;   - 첫 번째 매개변수(num): 변환할 정수 숫자 (ebp + 8)
;   - 두 번째 매개변수(pOutBytes): 아스키 문자로 변환된 숫자를 저장할 포인터 (ebp + 12)
; 반환값: 아스키 문자로 변환된 숫자 개수
;
; num에 범위는 다음과 같습니다.
; 0 <= num < 2147483647
;=========================================
IntegerToAscii2:
    ; 지역 변수:
    ;   - digit (ebp - 4)

    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 4
    push ebx
    push edx
    push edi

    ; num == 0인 경우,
    ; '0' 저장 후, 종료
    mov ebx, [ebp + 8]
    test ebx, ebx
    jnz lb_integer_to_ascii2

    mov al, '0'
    mov edi, [ebp + 12]         ; edi = pOutBytes
    mov [edi], al  ; pOutBytes[0] = '0'

    mov eax, 1
    mov [ebp - 4], eax

    jmp lb_integer_to_ascii2_return

lb_integer_to_ascii2:
    mov eax, [ebp + 8]          ; eax = num
    push eax
    call GetDigit
    mov [ebp - 4], eax

    mov edi, [ebp + 12]         ; edi = pOutBytes
    add edi, eax
    dec edi

    mov ebx, [ebp + 8]          ; ebx = num
    mov ecx, eax
lb_convert_to_string:
    test ebx, ebx
    jz lb_integer_to_ascii2_return

    mov eax, ebx        ; eax = num
    xor edx, edx
    mov ebx, 10
    div ebx             ; eax = num / 10

    add edx, '0'
    mov [edi], dl
    dec edi

    mov ebx, eax

    loop lb_convert_to_string

lb_integer_to_ascii2_return:
    mov eax, [ebp - 4]

    ; 에필로그
    pop edi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 8

;=========================================
; 함수 이름: GetDigit
; 설명: 이 함수는 정수의 자릿수를 알려줍니다.
; 매개변수:
;   - num (ebp + 8)
; 반환값: 자릿수
;=========================================
GetDigit:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    mov ebx, [ebp + 8]  ; ebx = num

    xor ecx, ecx
lb_calc_digit_loop:
    test ebx, ebx
    jz lb_get_digit_return

    inc ecx

    mov eax, ebx        ; eax = num
    xor edx, edx
    mov ebx, 10
    div ebx             ; eax = num / 10

    mov ebx, eax

    jmp lb_calc_digit_loop

lb_get_digit_return:
    ; 에필로그
    mov eax, ecx

    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 4