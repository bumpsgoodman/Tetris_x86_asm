EXTRN GetAsyncKeyState@4 : PROC

.data

.code

;=========================================
; 함수 이름: UpdateKey
; 설명: 이 함수는 키 상태를 업데이트합니다.
; 매개변수:
;   - virtualKey: 가상 키 코드 (ebp + 8)
;   - pKeyState: 키 상태 포인터 (ebp + 12)
; 반환값: 없음
;=========================================
UpdateKey:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx
    push esi
    push edi

    mov edi, [ebp + 12] ; edi = pKeyState

    mov eax, [ebp + 8]  ; eax = virtualKey
    push eax
    call GetAsyncKeyState@4

    ; key가 눌려있는지 확인
    test eax, eax
    jz lb_key_up

    ; 이전 상태가 UP인지 확인
    mov eax, [edi]
    cmp eax, KEY_STATE_UP
    jne lb_key_pressed

lb_key_down:
    mov eax, KEY_STATE_DOWN
    mov [edi], eax
    jmp lb_update_key_return

lb_key_pressed:
    mov eax, KEY_STATE_PRESSED
    mov [edi], eax
    jmp lb_update_key_return

lb_key_up:
    mov eax, KEY_STATE_UP
    mov [edi], eax

lb_update_key_return:
    ; 에필로그
    pop edi
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 8

;=========================================
; 함수 이름: tryBlockToBoard
; 설명: 이 함수는 블럭을 보드에 넣기를 시도합니다.
; 매개변수:
;   - x: 블럭 x 좌표 (ebp + 8)
;   - y: 블럭 y 좌표 (ebp + 12)
;   - block: 블럭 모양 (ebp + 16)
;   - state: 보드에 넣을 상태 (ebp + 20)
;   - pOffsets: 오프셋 포인터 (ebp + 24)
; 반환값: 보드에 넣었다면 1, 넣지 못했다면 0 반환
;=========================================
tryBlockToBoard:
    ; 지역 변수:
    ;   - actualX[NUM_OFFSETS] (ebp - 12)
    ;   - actualY[NUM_OFFSETS] (ebp - 24)

    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 24
    push ebx
    push edx
    push esi
    push edi

    ; 올바른 좌표인지 검사
    mov ebx, [ebp + 8]      ; ebx = x
    mov edx, [ebp + 12]     ; edx = y
    push edx    ; y
    push ebx    ; x
    call isValidPos

    ; x, y 위치가 올바르지 않다면 0 반환
    test eax, eax
    jz lb_try_block_to_board_return

    ; 실제 보드에 들어갈 수 있는지 검사
    ;-----------------------------------

    ; esi = 블럭 오프셋
    mov esi, [ebp + 24]

    lea edi, [ebp - 4]      ; edi = &actualX[0]
    lea edx, [ebp - 16]     ; edx = &actualY[0]

    mov ecx, NUM_OFFSETS
lb_calculate_actual_pos:
    mov eax, [ebp + 12]     ; eax = y
    mov bl, [esi + 1]       ; ebx = offsetY
    add al, bl              ; eax = y + offsetY
    mov [edx], eax          ; actualY[i] = y + offsetY

    push eax                ; 올바른 위치인지 검사하기 위해 계산된 y 좌표 push
    
    mov eax, [ebp + 8]      ; eax = x
    mov bl, [esi]           ; ebx = offsetX
    add al, bl              ; eax = x + offsetX
    mov [edi], eax          ; actualX[i] = x + offsetX

    push eax                ; 올바른 위치인지 검사하기 위해 계산된 x 좌표 push
    call isValidPos

    ; 올바르지 않은 위치면 반환
    test eax, eax
    jz lb_try_block_to_board_return

    add esi, 2              ; 다음 오프셋으로 이동
    sub edi, 4              ; 다음 actualX로 이동
    sub edx, 4              ; 다음 actualY로 이동

    loop lb_calculate_actual_pos

    ;-----------------------------------

    ; 실제 보드에 블럭 넣기
    ;-----------------------------------

    lea edi, board          ; edi = &board
    mov ebx, [ebp + 8]      ; ebx = x
    mov ecx, [ebp + 12]     ; ecx = y

    ; edi = &board[y][x] = board + y * BOARD_COLS + x
    mov eax, BOARD_COLS
    imul eax, ecx
    add eax, ebx
    add edi, eax

    ; board[y][x] = state
    mov eax, [ebp + 20]
    mov [edi], al

    lea ebx, [ebp - 4]      ; ebx = &actualX[0]
    lea esi, [ebp - 16]     ; esi = &actualY[0]

    mov ecx, NUM_OFFSETS
lb_try_to_board_loop:
    push ecx                ; ecx 백업

    lea edi, board          ; edi = &board

    mov eax, BOARD_COLS     ; eax = BOARD_COLS
    mov ecx, [esi]          ; ecx = actualY[i]
    imul eax, ecx
    
    mov ecx, [ebx]          ; ecx = actualX[i]
    add eax, ecx
    
    ; edi = &board[actualY[i]][actualX[i]] = board + y * BOARD_COLS + x
    add edi, eax

    ; board[actualY[i]][actualX[i]] = state
    mov eax, [ebp + 20]
    mov [edi], al

    sub ebx, 4
    sub esi, 4

    pop ecx
    loop lb_try_to_board_loop

    ;-----------------------------------

    mov eax, 1

lb_try_block_to_board_return:
    ; 에필로그
    pop edi
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 20

;=========================================
; 함수 이름: isValidPos
; 설명: 이 함수는 좌표가 올바른지 검사합니다.
; 매개변수:
;   - x: 블럭 x 좌표 (ebp + 8)
;   - y: 블럭 y 좌표 (ebp + 12)
; 반환값: 올바른 좌표라면 1, 그렇지 않다면 0 반환
;=========================================
isValidPos:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx
    push esi

    xor eax, eax

    ; x >= BOARD_COLS 검사
    mov ebx, [ebp + 8]          ; ebx = x
    cmp ebx, BOARD_COLS
    jge lb_is_valid_pos_return

    ; x < 0 검사
    cmp ebx, 0
    jl lb_is_valid_pos_return

    ; y >= BOARD_ROWS 검사
    mov edx, [ebp + 12]         ; edx = y
    cmp edx, BOARD_ROWS
    jge lb_is_valid_pos_return

    mov eax, BOARD_COLS         ; eax = BOARD_COLS
    mul edx                     ; eax = y * BOARD_COLS
    add eax, ebx                ; eax = (y * BOARD_COLS) + x
    
    ; esi = &board[y][x] = &board[y * BOARD_COLS + x]
    lea esi, board
    add esi, eax

    xor eax, eax

    ; board[y][x] == BOARD_STATE_FIXED 검사
    mov bl, [esi]              ; bl = board[y][x]
    mov dl, BOARD_STATE_FIXED
    cmp bl, dl
    jle lb_is_valid_pos_test2

    jmp lb_is_valid_pos

    lb_is_valid_pos_test2:
        cmp bl, BOARD_STATE_BLOCK
        jg lb_is_valid_pos_return

lb_is_valid_pos:

    inc eax

lb_is_valid_pos_return:
    ; 에필로그
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 8