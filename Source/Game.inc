.data

BOARD_ROWS  EQU     22
BOARD_COLS  EQU     10

BOARD_STATE_SPACE   EQU 0
BOARD_STATE_BLOCK   EQU 1
BOARD_STATE_FIXED   EQU 2
BOARD_STATE_SHADOW  EQU 3

NUM_BLOCK_SHAPES    EQU 7

NUM_OFFSETS     EQU 3
BLOCK_OFFSETS   DB    -1,  0,     1,  0,      2,  0      ; I
                DB    -1,  0,     0, -1,     -1, -1      ; O
                DB    -1,  0,    -1, -1,      1,  0      ; L
                DB    -1,  0,     1,  0,      1, -1      ; J
                DB    -1,  0,     1,  0,      0, -1      ; T
                DB    -1,  0,     0, -1,      1, -1      ; S
                DB     1,  0,     0, -1,     -1, -1      ; Z

JLSZT_TEST      DB     0,  0,     0,  0,      0,  0,      0,  0
                DB    -1,  0,     1,  0,      1,  0,     -1,  0
                DB    -1,  1,     1, -1,      1,  1,     -1, -1
                DB     0, -2,     0,  2,      0, -2,      0,  2
                DB    -1, -2,     1,  2,      1, -2,     -1,  2

I_TEST          DB     0,  0,     0,  0,      0,  0,      0,  0
                DB    -2,  0,    -1,  0,      2,  0,      1,  0
                DB     1,  0,     2,  0,     -1,  0,     -2,  0
                DB    -2, -1,    -1,  2,      2,  1,      1, -2
                DB     1,  2,     2, -1,     -1, -2,     -2,  1

shapeFlag           DB 0 ; getNextBlockInBundle 전용 변수

board               DB BOARD_ROWS * BOARD_COLS DUP (BOARD_STATE_SPACE)

blockBundle         DB 7 DUP (?)
blockBundleIndex    DB 0

block               DB ?
blockX              DB ?
blockY              DB ?

shadowX             DB ?
shadowY             DB ?

curOffsets          DB 6 DUP (?)    ; NUM_OFFSETS * 2 (x, y)
rotationCount       DB 0

KEY_STATE_UP        EQU     0
KEY_STATE_DOWN      EQU     1
KEY_STATE_PRESSED   EQU     2

VK_LEFT             EQU     025h
VK_RIGHT            EQU     027h
VK_UP               EQU     026h
VK_DOWN             EQU     028h
VK_SPACE            EQU     020h

leftKeyState        DD   0
rightKeyState       DD   0
upKeyState          DD   0
downKeyState        DD   0
spaceKeyState       DD   0

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
    push ebx
    push edx
    push esi
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

    call setNextBlockPos

    ; 오프셋 초기화
    ; esi = 블럭 오프셋
    lea esi, BLOCK_OFFSETS  ; esi = &BLOCK_OFFSETS
    movzx ebx, block        ; ebx = block
    mov eax, 6
    imul eax, ebx           ; eax = BLOCK_OFFSETS[block]
    add esi, eax            ; esi = &BLOCK_OFFSETS[block][0]

    lea edi, curOffsets     ; edi = curOffsets

    mov ecx, 6
lb_init_cur_offsets:
    movsb
    loop lb_init_cur_offsets

    movzx eax, block
    movzx ebx, blockX
    movzx edx, blockY
    push BOARD_STATE_BLOCK
    push eax
    push edx
    push ebx
    call tryBlockToBoard

lb_init_game_return:
    ; 에필로그
    pop edi
    pop esi
    pop edx
    pop ebx
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
    ; 지역 변수:
    ;   - nextX (ebp - 4)
    ;   - nextY (ebp - 8)

    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 8
    push ebx
    push edx
    push esi
    push edi

    ; nextX 초기화
    movzx eax, blockX
    mov [ebp - 4], eax

    ; nextY 초기화
    movzx eax, blockY
    mov [ebp - 8], eax

    ; 그림자 그리기
    ; -------------------------------------

    xor eax, eax
    mov al, BOARD_STATE_SPACE
    push eax
    movzx eax, block
    push eax
    movzx eax, blockY
    push eax
    movzx eax, blockX
    push eax
    call tryBlockToBoard

    ; -------------------------------------

    ; 키 업데이트
    ; -------------------------------------

    ; 왼쪽 방향키 업데이트
    lea edi, leftKeyState
    mov eax, VK_LEFT
    push edi            ; keyState
    push eax            ; 가상 키 코드
    call UpdateKey

    ; 오른쪽 방향키 업데이트
    lea edi, rightKeyState
    mov eax, VK_RIGHT
    push edi            ; keyState
    push eax            ; 가상 키 코드
    call UpdateKey

    ; 위쪽 방향키 업데이트
    lea edi, upKeyState
    mov eax, VK_UP
    push edi            ; keyState
    push eax            ; 가상 키 코드
    call UpdateKey

    ; 아래쪽 방향키 업데이트
    lea edi, downKeyState
    mov eax, VK_DOWN
    push edi            ; keyState
    push eax            ; 가상 키 코드
    call UpdateKey

    ; 스페이스바 업데이트
    lea edi, spaceKeyState
    mov eax, VK_SPACE
    push edi            ; keyState
    push eax            ; 가상 키 코드
    call UpdateKey

    ; -------------------------------------

    ; 키 처리
    ; -------------------------------------

    mov ebx, [ebp - 4] ; ebx = nextX
    mov edx, [ebp - 8] ; edx = nextY

lb_left_key:
    cmp leftKeyState, KEY_STATE_DOWN
    jne lb_right_key

    dec ebx
    jmp lb_update

lb_right_key:
    cmp rightKeyState, KEY_STATE_DOWN
    jne lb_up_key

    inc ebx
    jmp lb_update

lb_up_key:
    cmp upKeyState, KEY_STATE_DOWN
    jne lb_down_key

    movzx eax, [block]
    cmp eax, 1          ; O 블럭이면 회전 x
    je lb_update

    call rotate

    ; rotate가 실패했으면 바로 업데이트로 점프
    test eax, eax
    jz lb_update

    ; rotate가 성공했으면 위치 업데이트
    movzx ebx, blockX
    movzx edx, blockY

    jmp lb_update

lb_down_key:
    cmp downKeyState, KEY_STATE_DOWN
    jne lb_space_key

    inc edx
    jmp lb_update

lb_space_key:
    cmp spaceKeyState, KEY_STATE_DOWN
    jne lb_update

    ; -------------------------------------

    ; nextX, nextY 업데이트
    mov [ebp - 4], ebx
    mov [ebp - 8], edx

lb_update:
    ; 블럭 그리기
    xor eax, eax
    mov al, BOARD_STATE_BLOCK
    push eax    ; BOARD_STATE
    movzx eax, block
    push eax    ; block
    push edx    ; nextY
    push ebx    ; nextX
    call tryBlockToBoard

    ; 블럭 그리기 실패했으면, 고정
    ; ----------------------------------------

    test eax, eax
    jz lb_fixed

    ; blockX = nextX
    mov [blockX], bl

    ; blockY = nextY
    mov [blockY], dl

    jmp lb_update_game_return

    ; ----------------------------------------

lb_fixed:
    cmp leftKeyState, KEY_STATE_DOWN
    je lb_update_game_return

    cmp rightKeyState, KEY_STATE_DOWN
    je lb_update_game_return

    ; 블럭 고정 시키기
    push BOARD_STATE_FIXED
    xor eax, eax
    mov al, [block]
    push eax
    mov al, [blockY]
    push eax
    mov al, [blockX]
    push eax
    call TryBlockToBoard

    call removeLine

    call getNextBlock
    mov [block], al

    call setNextBlockPos

    ; 오프셋 업데이트
    ; esi = 블럭 오프셋
    lea esi, BLOCK_OFFSETS  ; esi = &BLOCK_OFFSETS
    movzx ebx, block        ; ebx = block
    mov eax, 6
    imul eax, ebx           ; eax = BLOCK_OFFSETS[block]
    add esi, eax            ; esi = &BLOCK_OFFSETS[block][0]

    lea edi, curOffsets     ; edi = curOffsets

    mov ecx, 6
lb_update_cur_offsets:
    movsb
    loop lb_update_cur_offsets

    ; 회전수 초기화
    xor eax, eax
    mov rotationCount, al

lb_update_game_return:
    ; 에필로그
    pop edi
    pop esi
    pop edx
    pop ebx
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

;=========================================
; 함수 이름: setNextBlockPos
; 설명: 이 함수는 다음 블럭의 위치를 세팅해주는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
setNextBlockPos:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx

    ; eax = BOARD_COLS / 2
    mov eax, BOARD_COLS
    mov ebx, 2
    div bl

    ; blockX = BOARD_COLS / 2 - 1
    dec eax
    mov [blockX], al

    xor ebx, ebx    ; ebx = blockY = 0
    mov al, [block]
    cmp al, 1
    jle lb_set_next_block_pos

    ; y = 1
    inc bl

lb_set_next_block_pos:
    ; blockY = bl
    mov [blockY], bl

lb_set_next_block_pos_return:
    ; 에필로그
    pop ebx
    mov esp, ebp
    pop ebp
    ret

; TODO: 회전 코드 전체적으로 검토
;=========================================
; 함수 이름: rotate
; 설명: 이 함수는 블럭을 회전하는 함수입니다.
; 매개변수: 없음
; 반환값: 회전에 성공했다면 1, 실패했다면 0 반환
;=========================================
rotate:
    ; 지역 변수:
    ;   - tempX (ebp - 4)     - unused
    ;   - tempY (ebp - 8)     - unused
    ;   - testX (ebp - 12)
    ;   - testY (ebp - 16)
    ;   - ecxBackup (ebp - 20)
    ;   - count (ebp - 24)
    ;   - result (ebp - 28)
    ;   - resultOffsets (ebp - 34)
    ;   - temp0 (ebp - 35)
    ;   - temp1 (ebp - 36)
    ;   - esiBackup (ebp - 40)

    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 40
    push ebx
    push edx
    push esi
    push edi

    ; result
    xor eax, eax
    mov [ebp - 28], eax

    mov ecx, 5
    mov [ebp - 20], ecx
    
    ; testTablePtr(esi) 구하기
    movzx eax, [block]  ; eax = block
    test eax, eax
    jz lb_i_test

    lea esi, JLSZT_TEST     ; esi = JLSZT_TEST
    mov [ebp - 40], esi     ; esi 백업
    
    jmp lb_test_loop0

lb_i_test:
    lea esi, I_TEST         ; esi = I_TEST
    mov [ebp - 40], esi     ; esi 백업

lb_test_loop0:
    mov esi, [ebp - 40]
    movzx eax, rotationCount    ; eax = rotationCount
    mov ebx, 2
    imul eax, ebx
    add esi, eax                ; esi = &testTablePtr[i][rotatinoCount]

    ; testX, testY 구하기
    xor ebx, ebx
    xor edx, edx
    mov bl, [esi]       ; bl = testTablePtr[i][rotatinoCount].x => testX
    mov [ebp - 12], bl
    mov bl, [esi + 1]   ; bl = testTablePtr[i][rotatinoCount].y => testY
    mov [ebp - 16], bl

    ; testTablePtr 다음 인덱스로 이동
    mov esi, [ebp - 40]
    add esi, 8
    mov [ebp - 40], esi

    lea edi, [ebp - 34]

    mov ecx, NUM_OFFSETS
    xor eax, eax
    mov [ebp - 24], eax       ; count = 0
    lea esi, curOffsets     ; esi = &curOffsets[i]
    lb_test_loop1:
        mov bl, [esi]           ; bl = curOffsets[i].x
        neg bl                  ; bl = -curOffsets[i].x

        mov dl, [esi + 1]       ; dl = curOffsets[i].y
        neg dl                  ; dl = -curOffsets[i].y

        add esi, 2

        ; -curOffsets[i].x, y 임시 저장
        mov [ebp - 35], bl
        mov [ebp - 36], dl

        xor eax, eax
        xor ebx, ebx

        ; -curOffsets[i].x * 0
        mov al, [ebp - 35]  ; al = -curOffsets[i].x
        xor ebx, ebx
        imul eax, ebx
        push eax    ; 연산 결과 백업

        ; -curOffsets[i].y * 1
        xor eax, eax
        mov al, [ebp - 36]   ; al = -curOffsets[i].y
        mov ebx, 1
        imul eax, ebx

        ; -curOffsets[i].x * 0 + -curOffsets[i].y * 1
        pop ebx
        add al, bl

        mov [edi], al   ; resultOffsets[i].x = 회전된 offsetX

        ; -curOffsets[i].x * -1
        xor eax, eax
        mov al, [ebp - 35]  ; al = -curOffsets[i].x
        mov ebx, -1
        imul eax, ebx
        push eax    ; 연산 결과 백업

        ; -curOffsets[i].y * 0
        xor eax, eax
        mov al, [ebp - 36]   ; al = -curOffsets[i].y
        xor ebx, ebx
        imul eax, ebx

        ; -curOffsets[i].x * 0 + -curOffsets[i].y * 1
        pop ebx
        add al, bl

        mov [edi + 1], al   ; resultOffsets[i].y = 회전된 offsetY

        ; eax = blockX + result_offset[i].x + test_pos.x
        movzx eax, blockX
        add al, [edi]
        add al, [ebp - 12]

        ; ebx = blockY + result_offset[i].y + test_pos.y
        movzx ebx, blockY
        add bl, [edi + 1]
        add bl, [ebp - 16]

        add edi, 2

        ; 회전 이후 범위를 벗어나거나
        ; 해당 위치에 이미 블럭이 있으면 회전 실패
        push ebx
        push eax
        call isValidPos

        test eax, eax
        jz exit_lb_test_loop1

        ; ++count
        mov eax, [ebp - 24]
        inc eax
        mov [ebp - 24], eax

        dec ecx
        test ecx, ecx
        jnz lb_test_loop1

exit_lb_test_loop1:
    mov eax, [ebp - 24] ; eax = count
    cmp eax, NUM_OFFSETS
    je lb_rotate_success

    mov ecx, [ebp - 20]
    dec ecx
    mov [ebp - 20], ecx ; ecx 백업
    test ecx, ecx
    jnz lb_test_loop0

    jmp lb_rotate_return

lb_rotate_success:
    ; offsets 업데이트
    lea esi, [ebp - 34]     ; esi = &resultOffsets
    lea edi, [curOffsets]   ; edi = &curOffsets
    mov ecx, 6
lb_update_rotate_offsets:
    movsb
    loop lb_update_rotate_offsets

    ; x 업데이트
    movzx eax, blockX   ; eax = blockX
    mov bl, [ebp - 12] ; ebx = testX
    add al, bl        ; eax = blockX + testX
    mov blockX, al

    ; y 업데이트
    movzx eax, blockY   ; eax = blockY
    mov bl, [ebp - 16] ; ebx = testY
    add al, bl        ; eax = blockY + testY
    mov blockY, al

    ; rotatinoCount = (rotationCount + 1) % 4
    movzx eax, [rotationCount]  ; eax = rotatinoCount
    inc eax
    mov bl, 4
    div bl
    mov [rotationCount], ah    ; rotationCount 업데이트

    mov eax, 1
    mov [ebp - 28], eax   ; result = 1

lb_rotate_return:
    mov eax, [ebp - 28] ; eax = result

    ; 에필로그
    pop edi
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: removeLine
; 설명: 이 함수는 지워져야 할 라인을 지웁니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
removeLine:
    ; 지역 변수:
    ;   - maxY (ebp - 4)

    ; 프롤로그
    push ebp
    mov ebp, esp
    sub esp, 4
    push ebx
    push edx
    push esi

    ; dl = maxY
    mov dl, -1

    ; esi = 블럭 오프셋
    lea esi, curOffsets
    ;lea esi, BLOCK_OFFSETS  ; esi = &BLOCK_OFFSETS
    ;mov ebx, [ebp + 16]     ; ebx = block
    ;mov eax, 6
    ;imul eax, ebx           ; eax = BLOCK_OFFSETS[block]
    ;add esi, eax            ; esi = &BLOCK_OFFSETS[block][0]
    ;inc esi                 ; esi = &BLOCK_OFFSETS[block][1] => y offset

    mov ecx, NUM_OFFSETS
lb_find_max_y:
    mov al, [blockY]        ; al = blockY
    add al, [esi]           ; al = blockY + offsetY
    cmp al, dl              ; maxY = max(maxY, blockY + offsetY)
    jle lb_no_greater_max_y

    mov dl, al

lb_no_greater_max_y:
    add esi, 2              ; 다음 오프셋으로 이동
    loop lb_find_max_y

    ; maxY = edx
    xor dh, dh
    ;mov [ebp - 4], edx

    mov ecx, 4
lb_remove_line:
    push ecx

    ; y가 1 이하로 떨어지면 종료
    cmp dl, 1
    jle lb_check_remove_line_cond

    lea esi, board  ; esi = &board[y]
    mov eax, BOARD_COLS
    imul eax, edx
    add esi, eax

    mov ecx, BOARD_COLS     ; ecx = BOARD_COLS
    xor ebx, ebx            ; ebx = count = 0
    lb_count_loop:
        mov al, [esi]       ; al = board[y][x]
        cmp al, BOARD_STATE_FIXED
        jne lb_no_fixed_block

        inc ebx     ; ++count

    lb_no_fixed_block:
        inc esi
        dec ecx
        test ecx, ecx
        jnz lb_count_loop

    cmp ebx, BOARD_COLS     ; 해당 행이 전부 채워졌다면
    jne lb_no_remove_line

    push edx
    call removeLineHelper

    jmp lb_check_remove_line_cond

lb_no_remove_line:
    dec dl  ; y 감소

lb_check_remove_line_cond:
    pop ecx
    dec ecx
    test ecx, ecx
    jnz lb_remove_line

lb_remove_line_return:
    ; 에필로그
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: removeLineHelper
; 설명: 이 함수는 removeLine 헬퍼 함수입니다.
;       실제로 board에서 라인을 지웁니다.
; 매개변수:
;   - y: y 좌표 (ebp + 8)
; 반환값: 없음
;=========================================
removeLineHelper:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push ebx
    push edx
    push esi
    push edi

    mov ebx, [ebp + 8]  ; ebx = y
    mov ecx, ebx        ; ecx = y
    dec ecx

    ; edi = &board[y]
    lea edi, board
    mov eax, BOARD_COLS
    imul eax, ebx
    add edi, eax

    ; esi = &board[y - 1]
    mov esi, edi
    sub esi, BOARD_COLS

lb_copy_line0:
    push ecx

    mov ecx, BOARD_COLS
    lb_copy_line1:
        mov al, [esi]   ; al = board[y - 1][i]
        mov [edi], al   ; board[y][i] = board[y - 1][i]

        inc esi
        inc edi

        loop lb_copy_line1

    sub esi, BOARD_COLS
    mov edi, esi
    sub esi, BOARD_COLS

    pop ecx
    loop lb_copy_line0

lb_remove_line_helper_return:
    ; 에필로그
    pop edi
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp
    ret 4