INCLUDE AsciiUtil.inc

EXTRN GetStdHandle@4 : PROC
EXTRN WriteConsoleA@20 : PROC

EXTRN GetConsoleMode@8 : PROC
EXTRN SetConsoleMode@8 : PROC

.data

STD_INPUT_HANDLE    DD ?    ; 표준 입력 핸들
STD_OUTPUT_HANDLE   DD ?    ; 표준 출력 핸들

;=========================================
; Console Virtual Terminal Sequences

SEQ_HIDE_CURSOR_LENGTH  DB  6
SEQ_HIDE_CURSOR         DB  01Bh, '[?25l', 0                    ; 커서 숨김 시퀀스

SEQ_ALTERNATE_BUFFER    DB  1Bh, '[1049h', 0                    ; 새 스크린 버퍼
SEQ_MAIN_BUFFER         DB  1Bh, '[1049l', 0                    ; 메인 스크린 버퍼

SEQ_CLEAR_SCREEN        DB  1Bh, '[2J', 0                       ; 화면 지우기
SEQ_ERASE_CHAR1         DB  1Bh, '[1X', 0                       ; 한 칸 지우기
SEQ_ERASE_CHAR2         DB  1Bh, '[2X', 0                       ; 두 칸 지우기
SEQ_MOVE_RIGHT_CURSOR1  DB  1Bh, '[1C', 0                       ; 커서 오른쪽으로 한 칸 이동
SEQ_MOVE_RIGHT_CURSOR2  DB  1Bh, '[2C', 0                       ; 커서 오른쪽으로 두 칸 이동

SEQ_DRAWING_MODE_LENGTH DB  3
SEQ_ASCII_DRAWING_MODE  DB  1Bh, '(B', 0                        ; ASCII 모드로 그리기
SEQ_LINE_DRAWING_MODE   DB  1Bh, '(0', 0                        ; DEC Line 모드로 그리기

;SEQ_KEY_UP_ARROW        DD   00415B1Bh
;SEQ_KEY_DOWN_ARROW      DD   00425B1Bh
;SEQ_KEY_RIGHT_ARROW     DD   00435B1Bh
;SEQ_KEY_LEFT_ARROW      DD   00445B1Bh
;SEQ_KEY_UP_ARROW        DB    1Bh, '[A', 0                        ; 위쪽 방향키
;SEQ_KEY_DOWN_ARROW      DB    1Bh, '[B', 0                        ; 아래쪽 방향키
;SEQ_KEY_RIGHT_ARROW     DB    1Bh, '[C', 0                        ; 오른쪽 방향키
;SEQ_KEY_LEFT_ARROW      DB    1Bh, '[D', 0                        ; 왼쪽 방향키

SEQ_TOP_LEFT_LINE       DB    'l', 0
SEQ_TOP_RIGHT_LINE      DB    'k', 0
SEQ_BOTTOM_LEFT_LINE    DB    'm', 0
SEQ_BOTTOM_RIGHT_LINE   DB    'j', 0
SEQ_VERTICAL_LINE       DB    'x', 0
SEQ_HORIZONTAL_LINE     DB    'q', 0

;=========================================

.code

;=========================================
; 함수 이름: InitConsole
; 설명: 이 함수는 콘솔 입출력을 위한 초기화를 진행합니다.
;       1. 커서 숨기기
;       2. STD_OUTPUT VT 모드 켜기
; 매개변수: 없음
; 반환값: 없음
;=========================================
InitConsole:
    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 8
    push ebx

    push -10                    ; STD_INPUT_HANDLE
    call GetStdHandle@4
    mov STD_INPUT_HANDLE, eax   ; STD_INPUT 저장

    push -11                    ; STD_OUTPUT_HANDLE
    call GetStdHandle@4
    mov STD_OUTPUT_HANDLE, eax  ; STD_OUTPUT 저장

    ; STD_OUTPUT 콘솔 모드 얻기
    lea eax, [ebp - 4]          ; eax = STD_OUTPUT_MODE가 저장될 포인터
    push eax                    ; lpMode, 현재 지정된 모드가 저장될 포인터
    push STD_OUTPUT_HANDLE      ; hConsoleHandle, 콘솔 핸들
    call GetConsoleMode@8

    ; STD_OUTPUT VT 모드 켜기
    mov eax, [ebp - 4]          ; eax = 현재 지정된 모드
    or eax, 4                   ; ENABLE_VIRTUAL_TERMINAL_PROCESSING
    push eax                    ; dwMode, 지정할 모드
    push STD_OUTPUT_HANDLE      ; hConsoleHandle, 콘솔 핸들
    call SetConsoleMode@8

    ; STD_INPUT 콘솔 모드 얻기
    ; lea eax, [ebp - 8]
    ; push eax
    ; push STD_INPUT_HANDLE
    ; call GetConsoleMode@8
    
    ; STD_INPUT VT 모드 켜기
    ; mov eax, [ebp - 8]
    ; or eax, 512 ; ENABLE_VIRTUAL_TERMINAL_INPUT
    ; mov ebx, 6
    ; not ebx
    ; and eax, ebx
    ; push eax
    ; push STD_INPUT_HANDLE
    ; call SetConsoleMode@8

    ; 커서 숨기기
    movzx eax, SEQ_HIDE_CURSOR_LENGTH   ; eax = 커서 숨기기 시퀀스 길이 저장
    lea ebx, SEQ_HIDE_CURSOR            ; ebx = 커서 숨기기 시퀀스 버퍼 포인터
    push eax                            ; messageLength
    push ebx                            ; pMessage
    call Print

lb_init_console_return:
    ; 에필로그
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: Print
; 설명: 이 함수는 메시지를 출력합니다.
; 매개변수:
;   - pMessage: 메시지가 저장된 포인터 (ebp + 8)
;   - messageLength: 메시지 길이 (ebp + 12)
; 반환값: 없음
;=========================================
Print:
    ; 프롤로그
    push ebp
    mov ebp, esp

    push 0                  ; lpReserved, NULL이어야 함
    push 0                  ; lpNumberOfCharsWritten, 실제로 쓰인 문자 수
    push [ebp + 12]         ; nNumberOfCharsToWrite, 쓸 문자 수
    push [ebp + 8]          ; lpBuffer, 쓸 문자 버퍼
    push STD_OUTPUT_HANDLE  ; hConsoleOutput, 콘솔 화면 핸들
    call WriteConsoleA@20

lb_print_return:
    ; 에필로그
    mov esp, ebp
    pop ebp
    ret 8

;=========================================
; 함수 이름: GotoXY
; 설명: 이 함수는 지정된 [x, y]로 커서 위치를 옮깁니다.
; 매개변수:
;   - x: x 좌표 (ebp + 8)
;   - y: y 좌표 (ebp + 12)
; 반환값: 없음
;=========================================
GotoXY:
    ; ESC[y;xH => 커서 위치 지정 시퀀스

    ; 지역변수:
    ;   - sequence[9]: 커서 위치 지정 시퀀스 버퍼 (ebp - 12)
    
    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 12
    push ebx
    push edi

    lea edi, [ebp - 12] ; edi = &sequence[0]

    mov al, 1Bh         ; ESC
    mov [edi], al
    mov al, '['
    mov [edi + 1], al
    add edi, 2

    ; y값 ascii로 변환
    mov eax, [ebp + 12]
    push edi
    push eax
    call IntegerToAscii
    add edi, eax

    mov al, ';'
    mov [edi], al
    inc edi

    ; x값 ascii로 변환
    mov al, [ebp + 8]
    push edi
    push eax
    call IntegerToAscii
    add edi, eax

    mov al, 'H'
    mov [edi], al
    inc edi

    ; 커서 이동하기
    lea ebx, [ebp - 12]
    sub edi, ebx        ; sequenceLength
    push edi
    push ebx
    call Print

lb_gotoxy_return:
    ; 에필로그
    pop edi
    pop ebx
    mov esp, ebp
    pop ebp
    ret 8

;=========================================
; 함수 이름: SetTextColor
; 설명: 이 함수는 지정된 [r, g, b]로 텍스트 색상을 변경합니다.
; 매개변수:
;   - r: red [0, 255] (ebp + 8)
;   - g: green [0, 255] (ebp + 12)
;   - b: blue [0, 255] (ebp + 16)
; 반환값: 없음
;=========================================
SetTextColor:
    ; ESC[38;2;r;g;bm -> 텍스트 색상 지정 시퀀스

    ; 지역변수
    ;   - sequence[21]: 텍스트 색상 지정 시퀀스 버퍼 (ebp - 24)

    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 24
    push ebx
    push edi

    lea edi, [ebp - 24] ; edi = &sequence[0]

    ; ESC[38;2; 삽입
    mov al, 1Bh         ; ESC
    mov [edi], al
    mov al, '['
    mov [edi + 1], al
    mov al, '3'
    mov [edi + 2], al
    mov al, '8'
    mov [edi + 3], al
    mov al, ';'
    mov [edi + 4], al
    mov al, '2'
    mov [edi + 5], al
    mov al, ';'
    mov [edi + 6], al
    add edi, 7

    ; r값 ascii로 변환 및 삽입
    mov eax, [ebp + 8]
    push edi
    push eax
    call IntegerToAscii
    add edi, eax

    ; ';' 삽입
    mov al, ';'
    mov [edi], al
    inc edi

    ; g값 ascii로 변환 및 삽입
    mov eax, [ebp + 12]
    push edi
    push eax
    call IntegerToAscii
    add edi, eax

    ; ';' 삽입
    mov al, ';'
    mov [edi], al
    inc edi

    ; b값 ascii로 변환 및 삽입
    mov eax, [ebp + 16]
    push edi
    push eax
    call IntegerToAscii
    add edi, eax

    ; 'm' 삽입
    mov al, 'm'
    mov [edi], al
    inc edi

    ; 색상 지정하기
    lea ebx, [ebp - 24]
    sub edi, ebx        ; sequenceLength
    push edi
    push ebx
    call Print

lb_set_text_color_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 12

;=========================================
; 함수 이름: SetBackColor
; 설명: 이 함수는 지정된 [r, g, b]로 배경 색상을 변경합니다.
; 매개변수:
;   - r: red [0, 255] (ebp + 8)
;   - g: green [0, 255] (ebp + 12)
;   - b: blue [0, 255] (ebp + 16)
; 반환값: 없음
;=========================================
SetBackColor:
    ; ESC[48;2;r;g;bm -> 텍스트 배경 지정 시퀀스

    ; 지역변수
    ;   - sequence[21]: 텍스트 배경 지정 시퀀스 버퍼 (ebp - 24)

    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 24
    push ebx
    push edi

    lea edi, [ebp - 24] ; edi = &sequence[0]

    ; ESC[48;2; 삽입
    mov al, 1Bh         ; ESC
    mov [edi], al
    mov al, '['
    mov [edi + 1], al
    mov al, '4'
    mov [edi + 2], al
    mov al, '8'
    mov [edi + 3], al
    mov al, ';'
    mov [edi + 4], al
    mov al, '2'
    mov [edi + 5], al
    mov al, ';'
    mov [edi + 6], al
    add edi, 7

    ; r값 ascii로 변환 및 삽입
    mov eax, [ebp + 8]
    push edi
    push eax
    call IntegerToAscii
    add edi, eax

    ; ';' 삽입
    mov al, ';'
    mov [edi], al
    inc edi

    ; g값 ascii로 변환 및 삽입
    mov eax, [ebp + 12]
    push edi
    push eax
    call IntegerToAscii
    add edi, eax

    ; ';' 삽입
    mov al, ';'
    mov [edi], al
    inc edi

    ; b값 ascii로 변환 및 삽입
    mov eax, [ebp + 16]
    push edi
    push eax
    call IntegerToAscii
    add edi, eax

    ; 'm' 삽입
    mov al, 'm'
    mov [edi], al
    inc edi

    ; 색상 지정하기
    lea ebx, [ebp - 24]
    sub edi, ebx        ; sequenceLength
    push edi
    push ebx
    call Print

lb_set_back_color_return:
    ; 에필로그
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 12

;=========================================
; 함수 이름: SetAsciiDrawingMode
; 설명: 이 함수는 콘솔 출력 모드를 ASCII로 변경합니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
SetAsciiDrawingMode:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx

    ; ascii darwing 모드로 변경
    lea eax, SEQ_ASCII_DRAWING_MODE
    movsx ebx, SEQ_DRAWING_MODE_LENGTH
    push ebx
    push eax
    call Print

lb_set_ascii_drawing_mode_return:
    ; 에필로그
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: SetLineDrawingMode
; 설명: 이 함수는 콘솔 출력 모드를 LINE으로 변경합니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
SetLineDrawingMode:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx

    ; line darwing 모드로 변경
    lea eax, SEQ_LINE_DRAWING_MODE
    movsx ebx, SEQ_DRAWING_MODE_LENGTH
    push ebx
    push eax
    call Print

lb_set_line_drawing_mode_return:
    ; 에필로그
    pop ebx
    mov esp, ebp
    pop ebp
    ret