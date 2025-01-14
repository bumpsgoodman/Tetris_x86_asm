.data

STATUS_SECTION_TITLE        DB  'STATUS'
STATUS_SECTION_TITLE_LENGTH DB  6

HELP_SECTION_TITLE          DB  'HELP'
HELP_SECTION_TITLE_LENGTH   DB  4

GAME_SECTION_TITLE          DB  0
GAME_SECTION_TITLE_LENGTH   DB  0

NEXT_SECTION_TITLE          DB  'NEXT'
NEXT_SECTION_TITLE_LENGTH   DB  4

.code

;=========================================
; 함수 이름: DrawStatusSection
; 설명: 이 함수는 STATUS 섹션을 그리는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
DrawStatusSection:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    ; status 섹션 그리기
    movzx ebx, STATUS_SECTION_TITLE_LENGTH
    lea edx, STATUS_SECTION_TITLE
    push ebx    ; titleLength
    push edx    ; pTitle
    push 8      ; bottomRightY
    push 30     ; bottomRightX
    push 1      ; topLeftY
    push 1      ; topLeftX
    call drawSection

lb_draw_status_section_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: DrawHelpSection
; 설명: 이 함수는 HELP 섹션을 그리는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
DrawHelpSection:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    ; help 섹션 그리기
    movzx ebx, HELP_SECTION_TITLE_LENGTH
    lea edx, HELP_SECTION_TITLE
    push ebx    ; titleLength
    push edx    ; pTitle
    push 16     ; bottomRightY
    push 30     ; bottomRightX
    push 9      ; topLeftY
    push 1      ; topLeftX
    call drawSection

lb_help_status_section_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: DrawGameSection
; 설명: 이 함수는 GAME 섹션을 그리는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
DrawGameSection:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    ; game 섹션 그리기
    movzx ebx, GAME_SECTION_TITLE_LENGTH
    lea edx, GAME_SECTION_TITLE
    push ebx    ; titleLength
    push edx    ; pTitle
    push 24     ; bottomRightY
    push 52     ; bottomRightX
    push 1      ; topLeftY
    push 31     ; topLeftX
    call drawSection

lb_game_status_section_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: DrawNextSection
; 설명: 이 함수는 GAME 섹션을 그리는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
DrawNextSection:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    ; next 섹션 그리기
    movzx ebx, NEXT_SECTION_TITLE_LENGTH
    lea edx, NEXT_SECTION_TITLE
    push ebx    ; titleLength
    push edx    ; pTitle
    push 26     ; bottomRightY
    push 69     ; bottomRightX
    push 1      ; topLeftY
    push 53     ; topLeftX
    call drawSection

lb_next_status_section_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: DrawBoard
; 설명: 이 함수는 보드를 그리는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
DrawBoard:
    ; 지역 변수:
    ;   - x (ebp - 4)
    ;   - y (ebp - 8)
    ;   - row (ebp - 12)
    ;   - col (ebp - 16)

    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 16
    push ebx
    push edx
    push esi

    mov ebx, 32
    mov [ebp - 4], ebx      ; x = 32

    mov ebx, 2
    mov [ebp - 8], ebx      ; y = 2

    lea esi, board          ; esi = &board

    mov ecx, BOARD_ROWS     ; ecx = BOARD_ROWS
lb_draw_board_loop0:
    push ecx                ; ecx 백업
    mov [ebp - 12], ecx     ; row = 0

    mov ecx, BOARD_COLS     ; ecx = BOARD_COLS
    lb_draw_board_loop1:
        push ecx            ; ecx 백업
        mov [ebp - 16], ecx

        ; 커서 조정
        mov ebx, [ebp - 4]      ; ebx = x
        mov edx, [ebp - 8]      ; edx = y
        push edx
        push ebx
        call GotoXY

        ; 블럭 상태 검사
        mov bl, [esi]
        cmp bl, BOARD_STATE_SPACE
        jne lb_test_state_block
        
        mov eax, [ebp - 12]     ; eax = row
        add eax, [ebp - 16]     ; eax = row + col
        mov ebx, 2
        div bx
        test dx, dx             ; eax % 2 == 0 검사
        jz lb_space_black

        push 48  ; blue
        push 48  ; green
        push 48  ; red
        call SetBackColor

        jmp lb_draw_state

        ; 상태가 공백인 경우
    lb_space_black:
        push 12  ; blue
        push 12  ; green
        push 12  ; red
        call SetBackColor

        jmp lb_draw_state

    lb_test_state_block:
        cmp bl, BOARD_STATE_BLOCK
        jne lb_test_state_fixed

        push 0      ; blue
        push 0      ; green
        push 255    ; red
        call SetBackColor

        jmp lb_draw_state

    lb_test_state_fixed:
        cmp bl, BOARD_STATE_FIXED
        jne lb_test_state_shadow

        push 255    ; blue
        push 0      ; green
        push 0      ; red
        call SetBackColor

        jmp lb_draw_state

    lb_test_state_shadow:
        ; shadow까지 아닌 경우는 고려하지 않음
        push 0      ; blue
        push 255    ; green
        push 0      ; red
        call SetBackColor

    lb_draw_state:
        lea ebx, SEQ_ERASE_CHAR2
        push 5                  ; TODO: 길이 하드 코딩 말고, 변수 따로 만들기
        push ebx
        call Print

        inc esi             ; 다음으로 그릴 대상으로 이동

        mov ebx, [ebp - 4]  ; ebx = x
        add ebx, 2          ; x증가
        mov [ebp - 4], ebx
        
        pop ecx
        dec ecx
        test ecx, ecx
        jnz lb_draw_board_loop1

    mov ebx, [ebp - 4]  ; ebx = x
    mov edx, [ebp - 8]  ; edx = y
    inc edx             ; y 증가
    mov ebx, 32         ; x 초기화
    mov [ebp - 4], ebx
    mov [ebp - 8], edx

    pop ecx
    dec ecx
    test ecx, ecx
    jnz lb_draw_board_loop0
    ; loop lb_draw_board_loop0

lb_draw_board_return:
    ; 기본 텍스트 배경 색상으로 변경
    push 12    ; blue
    push 12    ; green
    push 12    ; red
    call SetBackColor

    ; 에필로그
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: DrawNextBlocks
; 설명: 이 함수는 다음 블럭들을 그리는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
DrawNextBlocks:
    ; 지역 변수:
    ;   - x (ebp - 4)
    ;   - y (ebp - 8)
    ;   - index (ebp - 12)

    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 12
    push ebx
    push edx
    push esi

    ; 배경색 지정
    push 0    ; blue
    push 255    ; green
    push 255      ; red
    call SetBackColor

    ; x = 60
    mov eax, 60
    mov [ebp - 4], eax

    ; y = 4
    mov eax, 4
    mov [ebp - 8], eax

    ; index = blockBundleIndex
    movzx edx, blockBundleIndex
    mov [ebp - 12], edx

    mov ecx, 6
lb_draw_next_block:
    push ecx

    lea esi, blockBundle            ; esi = &blockBundle
    xor edx, edx
    mov dl, [ebp - 12]              ; edx = index
    add esi, edx                    ; esi = &blockBundle[index]

    ; ebx = blockBundle[index]
    mov bl, [esi]

    ; esi = 블럭 오프셋
    lea esi, BLOCK_OFFSETS  ; esi = &BLOCK_OFFSETS
    mov eax, 6
    mul bl                  ; eax = BLOCK_OFFSETS[block]
    add esi, eax            ; esi = &BLOCK_OFFSETS[block][0]

    mov ecx, NUM_OFFSETS
lb_calculate_actual_block_pos:
    push ecx

    ; y 오프셋 구하기
    mov edx, [ebp - 8]      ; edx = y
    mov ebx, [esi + 1]      ; ebx = offsetY
    add dl, bl              ; edx = y + offsetY

    ; x 오프셋 구하기
    mov eax, [esi]          ; eax = offsetX
    mov ebx, 2
    mul bl                  ; 오프셋에 * 2해서 출력할 때 보기 좋게 간격 넓히기
    mov ebx, [ebp - 4]      ; ebx = x
    add al, bl              ; eax = x + offsetX

    ; 커서 조정
    push edx    ; actualY
    push eax    ; actualX
    call GotoXY

    lea ebx, SEQ_ERASE_CHAR2
    push 5                  ; TODO: 길이 하드 코딩 말고, 변수 따로 만들기
    push ebx
    call Print

    add esi, 2              ; 다음 오프셋으로 이동

    pop ecx
    loop lb_calculate_actual_block_pos

    ; 커서 조정
    mov ebx, [ebp - 4]  ; x
    mov edx, [ebp - 8]  ; y
    push edx            ; y
    push ebx            ; x
    call GotoXY

    lea ebx, SEQ_ERASE_CHAR2
    push 5                  ; TODO: 길이 하드 코딩 말고, 변수 따로 만들기
    push ebx
    call Print

    ; 다음 블럭을 그리기 위한 y 증가
    mov eax, 4
    add [ebp - 8], eax

    ; 다음 index로 이동
    mov eax, [ebp - 12]
    inc eax
    mov edx, eax
    cmp eax, 7
    jne lb_no_overflow_index

    mov bl, 7
    div bl
    movzx edx, ah

lb_no_overflow_index:
    mov [ebp - 12], edx

    pop ecx
    dec ecx
    test ecx, ecx
    jnz lb_draw_next_block

lb_draw_next_blocks_return:
    ; 기본 텍스트 배경 색상으로 변경
    push 12    ; blue
    push 12    ; green
    push 12    ; red
    call SetBackColor

    ; 에필로그
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: drawSection
; 설명: 이 함수는 섹션을 그리는 함수입니다.
; 매개변수:
;   - topLeftX: 좌측 상단 x (ebp + 8)
;   - topLeftY: 좌측 상단 y (ebp + 12)
;   - bottomRightX: 우측 하단 x (ebp + 16)
;   - bottomRightY: 우측 하단 y (ebp + 20)
;   - pTitle: 섹션 제목 (ebp + 24)
;   - titleLength: 섹션 제목 길이 (ebp + 28)
; 반환값: 없음
;=========================================
drawSection:
    ; 지역변수:
    ;   - width (ebp - 4)
    ;   - height (ebp - 8)
    ;   - startTitleX (ebp - 12)
    ;   - endTitleX (ebp - 16)

    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx
    sub esp, 16

    ; width = x1 - x0
    mov eax, [ebp + 16]
    sub eax, [ebp + 8]
    mov [ebp - 4], eax

    ; height = x1 - x0
    mov eax, [ebp + 20]
    sub eax, [ebp + 12]
    mov [ebp - 8], eax

    ; al = width / 2 => 중간 위치
    mov eax, [ebp - 4]  ; eax = width
    mov bl, 2
    div bl
    movzx eax, al       ; div 나머지 결과가 ah에 저장되어 있어서, 몫만 남기고 다 0으로 초기화

    add al, [ebp + 8]   ; midX = startX + width / 2

    ; 타이틀 시작 위치, 끝 위치를 중간 위치로 초기화
    mov [ebp - 12], eax  ; startTitleX = midX
    mov [ebp - 16], eax  ; endTitleX = midX

    ; al = titleLength / 2
    mov eax, [ebp + 28] ; eax = titleLength
    mov bl, 2
    div bl              ; titleLength / 2
    movzx eax, al       ; div 나머지 결과가 ah에 저장되어 있어서, 몫만 남기고 다 0으로 초기화

    ; 타이틀 시작 위치, 끝 위치 구하기
    dec eax
    sub [ebp - 12], eax ; startTitleX = midX - titleLength / 2 - 1
    add eax, 1
    add [ebp - 16], eax ; endTitleX = midX + titleLength / 2

    ; 좌측 상단 코너 선 그리기
    lea ebx, SEQ_TOP_LEFT_LINE
    push 1
    push ebx
    push [ebp + 12]
    push [ebp + 8]
    call drawLine

    ; 좌측 하단 코너 선 그리기
    lea ebx, SEQ_BOTTOM_LEFT_LINE
    push 1
    push ebx
    push [ebp + 20]
    push [ebp + 8]
    call drawLine

    ; 우측 상단 코너 선 그리기
    lea ebx, SEQ_TOP_RIGHT_LINE
    push 1
    push ebx
    push [ebp + 12]
    push [ebp + 16]
    call drawLine

    ; 우측 하단 코너 선 그리기
    lea ebx, SEQ_BOTTOM_RIGHT_LINE
    push 1
    push ebx
    push [ebp + 20]
    push [ebp + 16]
    call drawLine

    ; 좌측 및 우측 수직선 그리기
    mov ecx, [ebp - 8]  ; ecx = height
    dec ecx             ; 코너 선 제외시키기
    mov edx, [ebp + 12] ; edx = topLeftY
    inc edx             ; 코너 선 제외시키기
    lea ebx, SEQ_VERTICAL_LINE  ; ebx = 수직선
lb_draw_vertical_line_loop:
    push ecx

    ; 좌측 수직선 그리기
    push 1
    push ebx
    push edx
    push [ebp + 8]      ; topLeftX
    call drawLine

    ; 우측 수직선 그리기
    push 1
    push ebx
    push edx
    push [ebp + 16]     ; bottomRightX
    call drawLine

    inc edx ; y 좌표 증가
    
    pop ecx
    loop lb_draw_vertical_line_loop

    ; 상단 및 하단 수평선 그리기
    mov ecx, [ebp - 4]  ; ecx = width
    dec ecx             ; 코너 선 제외시키기
    mov edx, [ebp + 8]  ; edx = topLeftX
    inc edx             ; 코너 선 제외시키기
    lea ebx, SEQ_HORIZONTAL_LINE  ; ebx = 수평선
lb_draw_horizontal_line_loop:
    push ecx

    cmp edx, [ebp - 12] ; edx >= startTitleX
    jl lb_draw_top_horizontal_line

    cmp edx, [ebp - 16] ; edx <= endTitleX
    jle lb_skip_top_horizontal_line

lb_draw_top_horizontal_line:
    ; 상단 수평선 그리기
    push 1
    push ebx
    push [ebp + 12]     ; topLeftY
    push edx            ; x
    call drawLine

lb_skip_top_horizontal_line:
    ; 하단 수평선 그리기
    push 1
    push ebx
    push [ebp + 20]     ; bottomRightY
    push edx            ; x
    call drawLine

    inc edx ; x 좌표 증가

    pop ecx
    loop lb_draw_horizontal_line_loop

    ; 타이틀 그리기
lb_draw_title:
    mov ebx, [ebp - 12] ; ebx = startTitleX

    ; title이 없는 경우, 타이틀 그리기 스킵
    cmp ebx, [ebp - 16]    ; startTitleX == endTitleX
    jz lb_skip_title

    ; 타이틀 그릴 위치로 커서 옮기기
    push [ebp + 12] ; topLeftY
    push ebx        ; startTitleX
    call GotoXY

    push [ebp + 28] ; titleLength
    push [ebp + 24] ; title
    call Print

    jmp lb_draw_section_return

    ; 타이틀 그리기 스킵
lb_skip_title:
    lea eax, SEQ_HORIZONTAL_LINE
    push 1
    push eax
    push [ebp + 12] ; topLeftY
    push ebx        ; startTitleX
    call drawLine

lb_draw_section_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 24

;=========================================
; 함수 이름: drawLine
; 설명: 이 함수는 선을 그리는 함수입니다.
; 매개변수:
;   - x: x 좌표 (ebp + 8)
;   - y: y 좌표 (ebp + 12)
;   - pLineSeq: 선 VT 시퀀스 (ebp + 16)
;   - lineSeqLength: 선 VT 시퀀스 길이 (ebp + 20)
; 반환값: 없음
;=========================================
drawLine:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    ; [x, y]로 커서 옮기기
    mov ebx, [ebp + 8]      ; x
    mov edx, [ebp + 12]     ; y
    push edx
    push ebx
    call GotoXY

    call SetLineDrawingMode     ; 선 그리기 모드로 변경

    ; 선 그리기
    push [ebp + 20]
    push [ebp + 16]
    call Print

    call SetAsciiDrawingMode    ; ASCII 그리기 모드로 변경

lb_draw_line_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 16

;=========================================
; 함수 이름: DrawBlock
; 설명: 이 함수는 블럭을 그리는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
DrawBlock:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx

    TODO:

lb_draw_block_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret