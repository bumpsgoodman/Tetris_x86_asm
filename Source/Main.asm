;=========================================
; 코딩 컨벤션
; 0. 호출 규약은 stdcall + edx 레지스터 보존입니다.
; 1. 자료형은 UPPER_CASE입니다.
; 2. main 함수를 제외한 모든 public 함수는 PascalCase입니다.
; 3. main 함수를 제외한 모든 private 함수는 cameCase입니다.
; 4. 레이블은 snake_case입니다.
; 5. 레이블은 접두사 "lb_"가 붙습니다.
; 6. 모든 함수는 반환 레이블을 넣습니다. 예) lb_main_return:
; 7. 주석을 최대한 자세하게 작성해야 합니다.
; 8. 반환값에 주석으로 비트 수가 적혀 있지 않다면 기본적으로 32비트입니다.
;=========================================

title Tetris_x86_assembly

.386
.model flat, stdcall
.stack 512

INCLUDE ConsoleUtil.inc
INCLUDE Game.inc

EXTRN ExitProcess@4 : PROC

.data
GAME_OVER_TEXT  DB 'GAME OVER !', 0
EXIT_KEY_TEXT   DB 'PRESS ENTER KEY...', 0

FRAME_TIME  EQU 33  ; 30 fps
start_time  DWORD   ?
end_time    DWORD   ?

VK_ENTER            EQU     00Dh
enterKeyState       DD   0

.code

;=========================================
; 함수 이름: main
; 설명: 이 함수는 진입점 함수입니다.
;       테트리스 게임 루프를 수행합니다.
; 매개변수: 없음
; 반환값: 0
;=========================================
main:
    ; 프롤로그
    push ebp
    mov ebp, esp
    push edx
    push edi

    call InitConsole    ; 게임 시작 전, 콘솔 초기화
    call InitGame       ; 게임 초기화

    ; 프레임 시작 시간
    call GetTickCount@0
    mov start_time, eax
lb_game_loop:
    call IsRunningGame  ; 게임 오버인지 확인하기
    test eax, eax
    jz lb_print_game_over

    call DrawGame       ; 게임 그리기

lb_wait_next_frame:
    call UpdateGame     ; 게임 로직 업데이트

    ; 프레임 끝 시간
    call GetTickCount@0
    mov [end_time], eax

    ; end_time - start_time
    mov ecx, [start_time]
    sub eax, ecx

    cmp eax, FRAME_TIME
    jl lb_wait_next_frame

    ; start_time = end_time
    mov eax, [end_time]
    mov [start_time], eax
    jmp lb_game_loop

lb_print_game_over:
    call DrawGame

    push 0
    push 0
    push 255
    call SetTextColor

    push 20
    push 7
    call GotoXY

    lea edx, GAME_OVER_TEXT
    push 12
    push edx
    call Print

    push 21
    push 7
    call GotoXY

    lea edx, EXIT_KEY_TEXT
    push 19
    push edx
    call Print

    ; 엔터키 업데이트
    lea edi, enterKeyState
    mov eax, VK_ENTER
    push edi            ; keyState
    push eax            ; 가상 키 코드
    call UpdateKey

    cmp enterKeyState, KEY_STATE_DOWN
    jne lb_print_game_over

lb_main_return:
    ; 에필로그
    pop edi
    pop edx
    mov esp, ebp
    pop ebp
    ret

    push 0
    call ExitProcess@4
END main