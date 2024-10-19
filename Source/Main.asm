title Tetris

.386
.model flat, stdcall

INCLUDE ConsoleUtil.inc

EXTRN ExitProcess@4 : PROC

.data
; VT 시퀀스
; -------------------------------------------------------------------------------------------------------

SEQ_ALTERNATE_BUFFER    BYTE    1Bh, '[1049h', 0                    ; 새 스크린 버퍼
SEQ_MAIN_BUFFER         BYTE    1Bh, '[1049l', 0                    ; 메인 스크린 버퍼

SEQ_CLEAR_SCREEN        BYTE    1Bh, '[2J', 0                       ; 화면 지우기
SEQ_ERASE_CHAR1         BYTE    1Bh, '[1X', 0                       ; 한 칸 지우기
SEQ_ERASE_CHAR2         BYTE    1Bh, '[2X', 0                       ; 두 칸 지우기
SEQ_MOVE_RIGHT_CURSOR1  BYTE    1Bh, '[1C', 0                       ; 커서 오른쪽으로 한 칸 이동
SEQ_MOVE_RIGHT_CURSOR2  BYTE    1Bh, '[2C', 0                       ; 커서 오른쪽으로 두 칸 이동

SEQ_ASCII_DRAWING_MODE  BYTE    1Bh, '(B', 0                        ; ASCII 모드로 그리기
SEQ_LINE_DRAWING_MODE   BYTE    1Bh, '(0', 0                        ; DEC Line 모드로 그리기

SEQ_TOP_LEFT_LINE       BYTE    'l', 0
SEQ_TOP_RIGHT_LINE      BYTE    'k', 0
SEQ_BOTTOM_LEFT_LINE    BYTE    'm', 0
SEQ_BOTTOM_RIGHT_LINE   BYTE    'j', 0
SEQ_VERTICAL_LINE       BYTE    'x', 0
SEQ_HORIZONTAL_LINE     BYTE    'q', 0

; STATUS
; -------------------------------------------------------------------------------------------------------

STATUS_SECTION_TEXT     BYTE    'STATUS', 0

; HELP
; -------------------------------------------------------------------------------------------------------

HELP_SECTION_TEXT     BYTE    'HELP', 0

; GAME
; -------------------------------------------------------------------------------------------------------

GAME_SECTION_TEXT       BYTE    0

BOARD_ROWS  EQU     22
BOARD_COLS  EQU     10

BOARD_STATE_SPACE   EQU     0
BOARD_STATE_BLOCK   EQU     1
BOARD_STATE_FIXED   EQU     2
BOARD_STATE_SHADOW  EQU     3

BLOCK_SHAPE_I   EQU 0
BLOCK_SHAPE_O   EQU 1
BLOCK_SHAPE_L   EQU 2
BLOCK_SHAPE_J   EQU 3
BLOCK_SHAPE_T   EQU 4
BLOCK_SHAPE_S   EQU 5
BLOCK_SHAPE_Z   EQU 6

board       BYTE   BOARD_ROWS * BOARD_COLS DUP (BOARD_STATE_SPACE)

blockShape  DWORD   ?
blockX      BYTE    ?
blockY      BYTE    ?



; NEXT
; -------------------------------------------------------------------------------------------------------

NEXT_SECTION_TEXT       BYTE    'NEXT', 0

.code

main PROC
    ; 초기화
    call InitConsole

    push OFFSET blockY
    push OFFSET blockX
    push blockShape
    call InitBlockPos

    lea eax, board
    mov BYTE PTR [eax + 63], BOARD_STATE_BLOCK
    mov BYTE PTR [eax + 64], BOARD_STATE_BLOCK
    mov BYTE PTR [eax + 54], BOARD_STATE_BLOCK
    mov BYTE PTR [eax + 55], BOARD_STATE_BLOCK

    call DrawBoard

lb_loop:
    ; status 섹션 그리기
    push LENGTHOF STATUS_SECTION_TEXT
    push OFFSET STATUS_SECTION_TEXT
    push 7
    push 18
    push 1
    push 1
    call DrawSection

    ; help 섹션 그리기
    push LENGTHOF HELP_SECTION_TEXT
    push OFFSET HELP_SECTION_TEXT
    push 24
    push 20
    push 8
    push 1
    call DrawSection

    ; game 섹션 그리기
    push LENGTHOF GAME_SECTION_TEXT
    push OFFSET GAME_SECTION_TEXT
    push 24
    push 47
    push 1
    push 25
    call DrawSection

    ; next 섹션 그리기
    push LENGTHOF NEXT_SECTION_TEXT
    push OFFSET NEXT_SECTION_TEXT
    push 24
    push 65
    push 1
    push 48
    call DrawSection

    push 50
    push 1
    call SetConsoleXY

    push 12
    push 12
    push 12
    call SetConsoleBackColor

    ;jmp lb_loop

    ; main 함수 종료
    push 0
    call ExitProcess@4
main ENDP

; shape에 따른 초기 x, y 위치 결정하는 함수
InitBlockPos PROC shape:DWORD, pOutX:PTR BYTE, pOutY: PTR BYTE
    xor edx, edx
    mov eax, BOARD_COLS
    mov ecx, 2
    div ecx                     ; BOARD_COLS / 2
    dec eax

    mov edx, pOutX
    mov BYTE PTR [edx], al      ; *pOutX = BOARD_COLS / 2 - 1

    mov eax, 2

    ; y는 I, O 모양인 경우 2, 나머지 모양인 경우 3
    cmp shape, 2
    jl lb_return

    inc eax
    
lb_return:
    mov edx, pOutY
    mov BYTE PTR [edx], al      ; *pOutY = 2 or 3
    ret
InitBlockPos ENDP

DrawBoard PROC
    LOCAL character[3]:BYTE
    LOCAL x:DWORD
    LOCAL y:DWORD

    ; ecx = row loop count
    ; eax = board[i]

    push ebx
    push esi
    push edi

    mov x, 27
    mov y, 2

    lea esi, board
    lea edi, character

    ; 널 문자 삽입
    mov BYTE PTR [edi + 1], ' '
    mov BYTE PTR [edi + 2], 0

    mov ecx, BOARD_ROWS
lb_loop0:
    push ecx

    push y
    push x
    call SetConsoleXY

    mov ecx, BOARD_COLS
    lb_loop1:
        push ecx

        mov al, BYTE PTR [esi]
        inc esi

        cmp al, BOARD_STATE_SPACE
        je lb_state_space

        cmp al, BOARD_STATE_BLOCK
        je lb_state_block

        cmp al, BOARD_STATE_FIXED
        je lb_state_fixed

        ; state shadow

    lb_state_space:
        push 255
        push 255
        push 255
        call SetConsoleTextColor

        push 12
        push 12
        push 12
        call SetConsoleBackColor

        mov BYTE PTR [edi], '.'

        push 3
        push edi
        call PrintConsoleMsg

        pop ecx
        dec ecx
        jnz lb_loop1

        jmp lb_loop1_exit

    lb_state_block:
        push 0
        push 0
        push 255
        call SetConsoleBackColor

        push LENGTHOF SEQ_ERASE_CHAR2
        push OFFSET SEQ_ERASE_CHAR2
        call PrintConsoleMsg

        push LENGTHOF SEQ_MOVE_RIGHT_CURSOR2
        push OFFSET SEQ_MOVE_RIGHT_CURSOR2
        call PrintConsoleMsg

        pop ecx
        dec ecx
        jnz lb_loop1

        jmp lb_loop1_exit

    lb_state_fixed:

        pop ecx
        dec ecx
        jnz lb_loop1

        jmp lb_loop1_exit

    lb_state_shadow:

        pop ecx
        dec ecx
        jnz lb_loop1

    lb_loop1_exit:
        inc y
        
        pop ecx
        dec ecx
        jnz lb_loop0

    jmp lb_return

lb_return:
    pop ebx
    pop edi
    pop esi

    ret
DrawBoard ENDP

DrawSection PROC topLeftX:DWORD, topLeftY:DWORD, bottomRightX:DWORD, bottomRightY:DWORD, pTitle:PTR BYTE, titleLength:DWORD
    LOCAL sectionWidth:DWORD
    LOCAL sectionHeight:DWORD
    LOCAL titleX:DWORD

    push edi
    push ebx

    ; 텍스트 색상 설정
    push 0
    push 255
    push 255
    call SetConsoleTextColor

    ; 배경 색상 설정
    push 128
    push 128
    push 0
    call SetConsoleBackColor

    ; sectionWidth 계산
    mov eax, bottomRightX
    sub eax, topLeftX
    add eax, titleLength
    mov sectionWidth, eax

    ; titleX 계산
    xor edx, edx
    mov ebx, 2
    div ebx             ; sectionWidth / 2
    mov titleX, eax
    xor edx, edx
    mov eax, titleLength
    mov ebx, 2
    div ebx             ; titleLength / 2
    mov ebx, titleX
    sub ebx, eax        ; titleX = (sectionWidth / 2) - (titleLength / 2)
    add ebx, topLeftX   ; titleX = topLeftX + (sectionWidth / 2) - (titleLength / 2)
    mov titleX, ebx

    ; sectionHeight 계산
    mov eax, bottomRightY
    sub eax, topLeftY
    mov sectionHeight, eax

    ; 위쪽 수평선 그리기
    mov eax, sectionWidth
    mov ebx, topLeftX
    mov edx, topLeftY
    push eax
    push edx
    push ebx
    call DrawHorizontalLine

    ; 아래쪽 수평선 그리기
    mov eax, sectionWidth
    mov edx, bottomRightY
    push eax
    push edx
    push ebx
    call DrawHorizontalLine

    ; 왼쪽 수직선 그리기
    push sectionHeight
    push topLeftY
    push topLeftX
    call DrawVerticalLine

    ; 오른쪽 수직선 그리기
    mov eax, topleftX
    add eax, sectionWidth
    dec eax
    push sectionHeight
    push topLeftY
    push eax
    call DrawVerticalLine

    ; 타이틀을 그리기 위해 콘솔 위치 설정
    mov eax, titleX
    mov ebx, topLeftY
    push ebx
    push eax
    call SetConsoleXY

    ; 타이틀 그리기
    mov eax, titleLength
    mov edi, pTitle
    push eax
    push edi
    call PrintConsoleMsg

    ; Line 모드로 변경
    push LENGTHOF SEQ_LINE_DRAWING_MODE
    push OFFSET SEQ_LINE_DRAWING_MODE
    call PrintConsoleMsg

    ; 왼쪽 상단 선 그리기
    mov eax, topLeftX
    mov ebx, topLeftY
    push ebx
    push eax
    call SetConsoleXY
    push LENGTHOF SEQ_TOP_LEFT_LINE
    push OFFSET SEQ_TOP_LEFT_LINE
    call PrintConsoleMsg

    ; 왼쪽 하단 선 그리기
    mov eax, topLeftX
    mov ebx, bottomRightY
    push ebx
    push eax
    call SetConsoleXY
    push LENGTHOF SEQ_BOTTOM_LEFT_LINE
    push OFFSET SEQ_BOTTOM_LEFT_LINE
    call PrintConsoleMsg

    ; 오른쪽 상단 선 그리기
    mov eax, topLeftX
    add eax, sectionWidth
    dec eax
    mov ebx, topLeftY
    push ebx
    push eax
    call SetConsoleXY
    push LENGTHOF SEQ_TOP_RIGHT_LINE
    push OFFSET SEQ_TOP_RIGHT_LINE
    call PrintConsoleMsg

    ; 오른쪽 하단 선 그리기
    mov eax, topLeftX
    add eax, sectionWidth
    dec eax
    mov ebx, bottomRightY
    push ebx
    push eax
    call SetConsoleXY
    push LENGTHOF SEQ_BOTTOM_RIGHT_LINE
    push OFFSET SEQ_BOTTOM_RIGHT_LINE
    call PrintConsoleMsg

    ; ASCII 모드로 변경
    push LENGTHOF SEQ_ASCII_DRAWING_MODE
    push OFFSET SEQ_ASCII_DRAWING_MODE
    call PrintConsoleMsg

    ; 기본 배경 색상으로 되돌리기
    push 0
    push 0
    push 0
    call SetConsoleBackColor

    ; 기본 텍스트 색상으로 되돌리기
    push 255
    push 255
    push 255
    call SetConsoleTextColor

    pop edi
    pop ebx
    
    ret
DrawSection ENDP

DrawHorizontalLine PROC x:DWORD, y:DWORD, count:DWORD
    push ebx

    ; 시작 x, 시작 y 지정
    mov ebx, x
    mov edx, y
    push edx
    push ebx
    call SetConsoleXY

    ; Line 모드로 변경
    push LENGTHOF SEQ_LINE_DRAWING_MODE
    push OFFSET SEQ_LINE_DRAWING_MODE
    call PrintConsoleMsg

    mov ecx, count

lb_loop:
    push ecx

    push LENGTHOF SEQ_HORIZONTAL_LINE
    push OFFSET SEQ_HORIZONTAL_LINE
    call PrintConsoleMsg

    pop ecx

    loop lb_loop
    
    ; ASCII 모드로 변경
    push LENGTHOF SEQ_ASCII_DRAWING_MODE
    push OFFSET SEQ_ASCII_DRAWING_MODE
    call PrintConsoleMsg

    pop ebx

    ret
DrawHorizontalLine ENDP

DrawVerticalLine PROC x:DWORD, y:DWORD, count:DWORD
    mov ecx, count
    mov edx, y

lb_loop:
    push ecx
    push edx

    ; 시작 x, 시작 y 지정
    push edx
    push x
    call SetConsoleXY

    ; Line 모드로 변경
    push LENGTHOF SEQ_LINE_DRAWING_MODE
    push OFFSET SEQ_LINE_DRAWING_MODE
    call PrintConsoleMsg

    push LENGTHOF SEQ_VERTICAL_LINE
    push OFFSET SEQ_VERTICAL_LINE
    call PrintConsoleMsg

    ; ASCII 모드로 변경
    push LENGTHOF SEQ_ASCII_DRAWING_MODE
    push OFFSET SEQ_ASCII_DRAWING_MODE
    call PrintConsoleMsg

    pop edx

    inc edx

    pop ecx
    loop lb_loop

    ret
DrawVerticalLine ENDP

END main