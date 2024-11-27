INCLUDE GameDraw.inc
INCLUDE Random.inc

.data

BOARD_ROWS  EQU     22
BOARD_COLS  EQU     10

BOARD_STATE_SPACE   EQU     0
BOARD_STATE_BLOCK   EQU     1
BOARD_STATE_FIXED   EQU     2
BOARD_STATE_SHADOW  EQU     3

NUM_BLOCK_SHAPES    EQU 7
BLOCK_SHAPE_I       EQU 0
BLOCK_SHAPE_O       EQU 1
BLOCK_SHAPE_L       EQU 2
BLOCK_SHAPE_J       EQU 3
BLOCK_SHAPE_T       EQU 4
BLOCK_SHAPE_S       EQU 5
BLOCK_SHAPE_Z       EQU 6

NUM_OFFSETS     EQU 3
BLOCK_OFFSETS   DB    -1, 0,  1,  0,  2,  0     ; I
                DB     1, 0,  0,  1,  1,  1     ; O
                DB    -1, 0, -1, -1,  1,  0     ; L
                DB    -1, 0,  1,  0,  1, -1     ; J
                DB    -1, 0,  1,  0,  0, -1     ; T
                DB    -1, 0,  0, -1,  1, -1     ; S
                DB     1, 0,  0, -1, -1, -1     ; Z

board           DB BOARD_ROWS * BOARD_COLS DUP (BOARD_STATE_SPACE)

blocksBundle0   DB 7 DUP (?)
blocksBundle1   DB 7 DUP (?)
pBlocksBundle   DD ?

blockShape      DB ?
blockX          DB ?
blockY          DB ?

shadowX         DB ?
shadowY         DB ?

.code

;=========================================
; 함수 이름: InitGame
; 설명: 이 함수는 게임 시작 전, 초기화를 진행하는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
InitGame:
    ; 프롤로그
    push ebp
    mov ebp, esp

    ; 0번 블럭 번들 생성
    lea eax, blocksBundle0
    push eax
    call makeBlocksBundle

    ; 1번 블럭 번들 생성
    lea eax, blocksBundle1
    push eax
    call makeBlocksBundle

    ; 현재 블럭 번들 포인터 초기화
    lea eax, blocksBundle0
    mov [pBlocksBundle], eax

    ; 블럭 초기 shape 설정
    mov eax, [pBlocksBundle]
    mov al, [eax]
    mov [blockShape], al
    inc pBlocksBundle

    ; TODO
    ; 1. 블럭 그리기
    ; 2. 다음 블럭 그리기

lb_init_game_return:
    ; 에필로그
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: UpdateGame
; 설명: 이 함수는 게임 로직을 업데이트하는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
UpdateGame:
    ; 프롤로그
    push ebp
    mov ebp, esp

lb_update_game_return:
    ; 에필로그
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: DrawGame
; 설명: 이 함수는 게임을 그리는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
DrawGame:
    ; 프롤로그
    push ebp
    mov ebp, esp

    ; 텍스트 색상 지정
    push 0      ; blue
    push 255    ; green
    push 255    ; red
    call SetTextColor

    ; 텍스트 배경 색상 지정
    push 128
    push 128
    push 0
    call SetBackColor

    ; 섹션 그리기
    call DrawStatusSection
    call DrawHelpSection
    call DrawGameSection
    call DrawNextSection

lb_draw_game_return:
    ; 기본 텍스트 색상으로 변경
    push 255    ; blue
    push 255    ; green
    push 255    ; red
    call SetTextColor

    ; 기본 텍스트 배경 색상으로 변경
    push 12    ; blue
    push 12    ; green
    push 12    ; red
    call SetBackColor

    ; 에필로그
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: IsRunning
; 설명: 이 함수는 게임이 종료되었는지 알려주는 함수입니다.
; 매개변수: 없음
; 반환값: 게임 오버라면 1, 아니라면 0 반환
;=========================================
IsRunningGame:
    ; 프롤로그
    push ebp
    mov ebp, esp

lb_is_running_game_return:
    xor eax, eax ; 임시적으로 0 반환

    ; 에필로그
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: makeBlocksBundle
; 설명: 이 함수는 블럭 번들을 생성해주는 함수입니다.
; 매개변수:
;   - pBlocksBundle: 블럭 번들 (ebp + 8)
; 반환값: 없음
;=========================================
makeBlocksBundle:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx
    push edi

    mov edi, [ebp + 8]  ; edi = pBlocksBundle
    xor edx, edx        ; edx = shapeFlag
lb_make_blocks_loop:
    ; edx가 2^7 - 1 과 같은 경우, 중복 없이 7개 다 만들어짐
    cmp edx, 07Fh
    je lb_make_blocks_bundle_return

    ; shape 랜덤으로 결정하기
    push NUM_BLOCK_SHAPES
    call GetRandomRange
    
    ; shape 플래그 설정
    mov ecx, eax
    mov ebx, 1
    shl ebx, cl     ; ebx = 1 << shape

    ; shapeFlag가 이미 설정되어 있으면 스킵
    ; => 이미 해당 shape이 존재함
    mov ecx, ebx
    and ecx, edx    ; (1 << shape) & shapeFlag
    jnz lb_make_blocks_loop

    mov [edi], eax  ; pBlocksBundle[i++] = shape
    inc edi

    or edx, ebx     ; shapeFlag = shapeFlag | shape

    jmp lb_make_blocks_loop

lb_make_blocks_bundle_return:
    ; 에필로그
    pop edi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 4