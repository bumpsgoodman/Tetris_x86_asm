.data

.code

;=========================================
; 함수 이름: UpdateBoard
; 설명: 이 함수는 보드를 업데이트하는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
UpdateBoard:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    ; 블럭을 보드에 넣기 시도

lb_update_board_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

; TODO: 전체 로직 검사
;=========================================
; 함수 이름: tryBlockToBoard
; 설명: 이 함수는 블럭을 보드에 넣기를 시도합니다.
; 매개변수:
;   - x: 블럭 x 좌표 (ebp + 8)
;   - y: 블럭 y 좌표 (ebp + 12)
;   - block: 블럭 모양 (ebp + 16)
;   - state: 보드에 넣을 상태 (ebp + 20)
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
    mov edx, [ebp + 16]     ; edx = y
    push edx    ; y
    push ebx    ; x
    call isValidPos

    ; x, y 위치가 올바르지 않다면 0 반환
    test eax, eax
    jz lb_try_block_to_board_return

    ; 실제 보드에 들어갈 수 있는지 검사
    ;-----------------------------------

    ; esi = 블럭 오프셋
    lea esi, BLOCK_OFFSETS  ; esi = &BLOCK_OFFSETS
    mov eax, 6
    mul block               ; eax = BLOCK_OFFSETS[block]
    add esi, eax            ; esi = &BLOCK_OFFSETS[block][0]

    lea edi, [ebp - 12]     ; edi = &actualX[NUM_OFFSETS - 1]
    lea edx, [ebp - 24]     ; edx = &actualY[NUM_OFFSETS - 1]

    mov ecx, NUM_OFFSETS
lb_calculate_actual_pos:
    movzx eax, blockY       ; eax = blockY
    mov ebx, [esi + 1]      ; ebx = offsetY
    add eax, ebx            ; eax = blockY + offsetY
    mov [edx], eax          ; actualY[ecx - 1] = blockY + offsetY

    push eax                ; 올바른 위치인지 검사하기 위해 계산된 y 좌표 push
    
    movzx eax, blockX       ; eax = blockX
    mov ebx, [esi]          ; ebx = offsetX
    add eax, ebx            ; eax = blockX + offsetX
    mov [edi], eax          ; actualX[ecx - 1] = blockX + offsetX

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

lb_try_block_to_board_return:
    ; 에필로그
    pop edi
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 16

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
    cmp ebx, esi
    jge lb_is_valid_pos_return

    ; y >= BOARD_ROWS 검사
    mov edx, [ebp + 12]         ; edx = y
    cmp edx, BOARD_ROWS
    jge lb_is_valid_pos_return

    mov eax, BOARD_COLS         ; esi = BOARD_COLS
    mul edx                     ; eax = y * BOARD_COLS
    add edx, ebx                ; eax = (y * BOARD_COLS) + x
    
    ; esi = &board[y][x] = &board[y * BOARD_COLS + x]
    lea esi, board
    add esi, eax

    xor eax, eax

    ; board[y][x] == BOARD_STATE_FIXED 검사
    mov ebx, [esi]              ; ebx = board[y][x]
    cmp ebx, BOARD_STATE_FIXED
    je lb_is_valid_pos_return

    inc eax

lb_is_valid_pos_return:
    ; 에필로그
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 8