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
    push 24     ; bottomRightY
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