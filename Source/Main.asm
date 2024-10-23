title Tetris

.386
.model flat, stdcall

INCLUDE ConsoleUtil.inc
INCLUDE Random.inc

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

SEQ_KEY_UP_ARROW        DWORD   00415B1Bh
SEQ_KEY_DOWN_ARROW      DWORD   00425B1Bh
SEQ_KEY_RIGHT_ARROW     DWORD   00435B1Bh
SEQ_KEY_LEFT_ARROW      DWORD   00445B1Bh
;SEQ_KEY_UP_ARROW        BYTE    1Bh, '[A', 0                        ; 위쪽 방향키
;SEQ_KEY_DOWN_ARROW      BYTE    1Bh, '[B', 0                        ; 아래쪽 방향키
;SEQ_KEY_RIGHT_ARROW     BYTE    1Bh, '[C', 0                        ; 오른쪽 방향키
;SEQ_KEY_LEFT_ARROW      BYTE    1Bh, '[D', 0                        ; 왼쪽 방향키

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

BLOCK_OFFSETS   BYTE    -1, 0, 1, 0, 2, 0           ; I
                BYTE    1, 0, 0, 1, 1, 1            ; O
                BYTE    -1, 0, -1, -1, 1, 0         ; L
                BYTE    -1, 0, 1, 0, 1, -1          ; J
                BYTE    -1, 0, 1, 0, 0, -1          ; T
                BYTE    -1, 0, 0, -1, 1, -1         ; S
                BYTE    1, 0, 0, -1, -1, -1         ; Z

board       BYTE   BOARD_ROWS * BOARD_COLS DUP (BOARD_STATE_SPACE)

blockShape  BYTE    ?
blockX      BYTE    ?
blockY      BYTE    ?

readBuffer  DWORD   ?

; NEXT
; -------------------------------------------------------------------------------------------------------

NEXT_SECTION_TEXT       BYTE    'NEXT', 0

nextBlockSet0   BYTE    7   DUP (?)
nextBlockSet1   BYTE    7   DUP (?)
nextBlockIndex  BYTE    ?

; timer
; -------------------------------------------------------------------------------------------------------

FRAME_TIME  EQU 33  ; 30 fps
start_time  DWORD   ?
end_time    DWORD   ?

.code

main PROC
    ; 초기화
    push LENGTHOF SEQ_ALTERNATE_BUFFER
    push OFFSET SEQ_ALTERNATE_BUFFER
    call PrintConsoleMsg

    call InitConsole
    call InitGame

    call DrawBoard
    call FlushConsoleInput

    ; 프레임 시작 시간
    call GetTickCount@0
    mov start_time, eax

lb_loop:
    call InputGame
    call UpdateGame
    call DrawGame

lb_wait_next_frame:
    ; 프레임 끝 시간
    call GetTickCount@0
    mov end_time, eax

    ; end_time - start_time
    mov ecx, start_time
    sub eax, ecx

    cmp eax, FRAME_TIME
    jl lb_wait_next_frame

    ; start_time = end_time
    mov eax, end_time
    mov start_time, eax
    jmp lb_loop

    ; main 함수 종료
    push LENGTHOF SEQ_MAIN_BUFFER
    push OFFSET SEQ_MAIN_BUFFER
    call PrintConsoleMsg

    push 0
    call ExitProcess@4
main ENDP

InitGame PROC
    push edi

    push OFFSET nextBlockSet0
    call GenerateBlockSet

    push OFFSET nextBlockSet1
    call GenerateBlockSet

    ; 첫 블럭 불러오기
    mov al, BYTE PTR [nextBlockSet0]
    mov blockShape, al

    ; 다음 블럭 세팅
    mov al, BYTE PTR [nextBlockSet1]
    mov BYTE PTR [nextBlockSet0], al

    mov nextBlockIndex, 1

    ; 블럭 초기 위치 세팅
    xor eax, eax
    mov al, blockShape
    push OFFSET blockY
    push OFFSET blockX
    push eax
    call InitBlockPos

    pop edi
    ret
InitGame ENDP

InputGame PROC
    push 4
    push OFFSET readBuffer
    call ReadConsoleMsg

    cmp eax, 0
    je lb_return

    ; 읽은 데이터 불러오기
    mov eax, readBuffer
    
check_left_arrow:
    ; 왼쪽 방향키가 눌렸는지 확인
    cmp eax, SEQ_KEY_LEFT_ARROW
    jne check_right_arrow
    push 0
    call ExitProcess@4

check_right_arrow:
    ; 오른쪽 방향키가 눌렸는지 확인
    cmp eax, SEQ_KEY_RIGHT_ARROW
    jne check_down_arrow
    push 0
    call ExitProcess@4

check_down_arrow:
    ; 아래쪽 방향키가 눌렸는지 확인
    cmp eax, SEQ_KEY_DOWN_ARROW
    jne check_up_arrow
    push 0
    call ExitProcess@4

check_up_arrow:
    ; 위쪽 방향키가 눌렸는지 확인
    cmp eax, SEQ_KEY_UP_ARROW
    jne lb_return
    push 0
    call ExitProcess@4

lb_return:
    call FlushConsoleInput
    ret
InputGame ENDP

UpdateGame PROC
    ; 현재 블록을 보드에 넣기 시도
    push BOARD_STATE_BLOCK
    xor eax, eax
    mov al, blockShape
    push eax
    mov al, blockY
    push eax
    mov al, blockX
    push eax
    call TryBlockToBoard
    
    ret
UpdateGame ENDP

DrawGame PROC
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

    call DrawBoard

    ret
DrawGame ENDP

GenerateBlockSet PROC pOutBlocks:PTR BYTE
    push ebx
    push edi

    xor edx, edx
    mov edi, pOutBlocks

lb_loop:
    ; edx가 2^7 - 1 과 같은 경우, 중복 없이 7개 다 만들어짐
    cmp edx, 07Fh
    je lb_return

    ; edx 백업
    push edx

    ; 0 ~ 6까지 랜덤값 생성
    push 7
    call GetRandomRange

    pop edx
    
    ; 1 << random number
    mov ecx, eax
    mov ebx, 1
    shl ebx, cl

    test ebx, edx
    jne lb_loop

    mov BYTE PTR [edi], al
    inc edi

    or edx, ebx

    jmp lb_loop

lb_return:
    pop ebx
    pop edi
    ret
GenerateBlockSet ENDP

; shape에 따른 초기 x, y 위치 결정하는 함수
InitBlockPos PROC shape:BYTE, pOutX:PTR BYTE, pOutY: PTR BYTE
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

; 반환값 - 성공 시 1, 아니면 0
TryBlockToBoard PROC x:BYTE, y:BYTE, shape:BYTE, state:BYTE
    LOCAL actualX:BYTE
    LOCAL actualY:BYTE

    push edi
    push esi

    ; TODO: 범위 검사
    
    ; TODO: 실제 보드에 들어갈 수 있는지 검사

    ; BLOCK_OFFSETS 구하기
    lea esi, BLOCK_OFFSETS
    mov eax, 6
    mul shape
    add esi, eax

    mov ecx, 3
lb_loop:
    ; actualX 구하기
    mov al, x
    mov dl, BYTE PTR [esi]
    add al, dl
    mov actualX, al

    ; actualY 구하기
    mov al, y
    mov dl, BYTE PTR [esi + 1]
    add al, dl
    mov actualY, al

    mov dl, state

    ; [actualX, actualY] 출력
    lea edi, board
    mov al, BOARD_COLS
    mul actualY
    add al, actualX
    add edi, eax
    mov BYTE PTR [edi], dl

    add esi, 2

    loop lb_loop

    ; [x, y] 출력
    lea edi, board
    mov al, BOARD_COLS
    mul y
    add al, x
    add edi, eax
    mov BYTE PTR [edi], dl

lb_return:
    pop edi
    pop esi
    ret
TryBlockToBoard ENDP

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