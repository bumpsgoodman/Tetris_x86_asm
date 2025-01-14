.data

BOARD_ROWS  EQU     22
BOARD_COLS  EQU     10

BOARD_STATE_SPACE   EQU 0
BOARD_STATE_BLOCK   EQU 1
BOARD_STATE_FIXED   EQU 2
BOARD_STATE_SHADOW  EQU 3

NUM_BLOCK_SHAPES    EQU 7
BLOCK_SHAPE_I       EQU 0
BLOCK_SHAPE_O       EQU 1
BLOCK_SHAPE_L       EQU 2
BLOCK_SHAPE_J       EQU 3
BLOCK_SHAPE_T       EQU 4
BLOCK_SHAPE_S       EQU 5
BLOCK_SHAPE_Z       EQU 6

NUM_OFFSETS     EQU 3
BLOCK_OFFSETS   DB    -1, 0,     1,  0,      2,  0      ; I
                DB    -1, 0,     0, -1,     -1, -1      ; O
                DB    -1, 0,    -1, -1,      1,  0      ; L
                DB    -1, 0,     1,  0,      1, -1      ; J
                DB    -1, 0,     1,  0,      0, -1      ; T
                DB    -1, 0,     0, -1,      1, -1      ; S
                DB     1, 0,     0, -1,     -1, -1      ; Z

shapeFlag           DB 0 ; getNextBlockInBundle 전용 변수

board               DB BOARD_ROWS * BOARD_COLS DUP (BOARD_STATE_SPACE)

blockBundle         DB 7 DUP (?)
blockBundleIndex    DB 0

block               DB ?
blockX              DB ?
blockY              DB ?

shadowX             DB ?
shadowY             DB ?

INCLUDE Random.inc
INCLUDE GameDraw.inc
INCLUDE GameUpdate.inc

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
    push edi

    ; 블럭 번들 생성
    lea edi, blockBundle
    mov ecx, NUM_BLOCK_SHAPES
lb_make_block_bundle_loop:
    push ecx    ; ecx 백업
    call getNextBlockInBundle   ; 번들의 다음 블럭 얻어오기
    pop ecx

    ; 블럭 번들에 블럭 저장
    mov [edi], al
    inc edi

    loop lb_make_block_bundle_loop

    ; 블럭 초기 설정
    call getNextBlock
    mov [block], al

    mov [blockX], 5
    mov [blockY], 6

    movzx eax, block
    push BOARD_STATE_BLOCK
    push eax
    push 6
    push 5
    call tryBlockToBoard

lb_init_game_return:
    ; 에필로그
    pop edi
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

    ; 보드 그리기
    call DrawBoard

    ; TODO: 다음 블럭 그리기
    call DrawNextBlocks

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
; 함수 이름: getNextBlockInBundle
; 설명: 이 함수는 다음 번들의 블럭 모양이 무엇인지 반환해주는 함수입니다.
; 매개변수: 없음
; 반환값: 다음 번들의 블럭 모양 (8비트)
;=========================================
getNextBlockInBundle:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx
    push edi

    lea edi, shapeFlag  ; edi = &shapeFlag
    mov edx, [edi]      ; edx = shapeFlag
lb_shape_flag_loop:
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
    jnz lb_shape_flag_loop

    or edx, ebx     ; edx = shapeFlag | shape
    cmp edx, 07Fh   ; shapeFlag == 0b0111 1111 인지 검사
    jne lb_get_next_block_in_bundle_return

    xor edx, edx    ; shapeFlag가 모두 활성화 되면 0으로 초기화

lb_get_next_block_in_bundle_return:
    mov [edi], edx  ; shapeFlag = shapeFlag | shape

    ; 에필로그
    pop edi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: getNextBlock
; 설명: 이 함수는 다음 블럭 모양이 무엇인지 반환해주는 함수입니다.
; 매개변수: 없음
; 반환값: 다음 블럭 모양 (8비트)
;=========================================
getNextBlock:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx
    push esi

    xor ebx, ebx

    lea esi, blockBundle        ; esi = &blockBundle
    mov bl, blockBundleIndex    ; bl = blockBundleIndex
    add esi, ebx                ; esi = &blockBundle[blockBundleIndex]

    mov dl, [esi]  ; dl = blockBundle[blockBundleIndex]
    
    ; blockBundle[blockBundleIndex] = 다음 블럭
    call getNextBlockInBundle
    mov [esi], al

    ; 블럭 번들 인덱스 설정
    inc bl
    cmp bl, NUM_BLOCK_SHAPES
    jne lb_get_next_block_return

    xor bl, bl  ; blockBundleIndex 0으로 초기화

lb_get_next_block_return:
    mov [blockBundleIndex], bl    

    mov al, dl

    ; 에필로그
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret