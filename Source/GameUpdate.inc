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

;=========================================
; 함수 이름: tryBlockToBoard
; 설명: 이 함수는 블럭을 보드에 넣기를 시도합니다.
; 매개변수:
;   - pBoard: 보드 (ebp + 8)
;   - x: 블럭 x 좌표 (ebp + 12)
;   - y: 블럭 y 좌표 (ebp + 16)
;   - block: 블럭 모양 (ebp + 20)
;   - pOffsets: 블럭 오프셋 (ebp + 24)
; 반환값: 보드에 넣었다면 1, 넣지 못했다면 0 반환
;=========================================
tryBlockToBoard:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    ; 올바른 좌표인지 검사
    TODO:

lb_try_block_to_board_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 20

;=========================================
; 함수 이름: isValidPos
; 설명: 이 함수는 좌표가 올바른지 검사합니다.
; 매개변수:
;   - pBoard: 보드 (ebp + 8)
;   - x: 블럭 x 좌표 (ebp + 12)
;   - y: 블럭 y 좌표 (ebp + 16)
; 반환값: 올바른 좌표라면 1, 그렇지 않다면 0 반환
;=========================================
isValidPos:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    ; 올바른 좌표인지 검사
    TODO:

lb_is_valid_pos_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 20