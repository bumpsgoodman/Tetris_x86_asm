;=========================================
; 코딩 컨벤션
; 0. 호출 규약은 stdcall + edx 레지스터 보존입니다.
; 1. 자료형은 UPPER_CASE입니다.
; 2. main 함수를 제외한 모든 public 함수는 PascalCase입니다.
; 3. main 함수를 제외한 모든 private 함수는 cameCase입니다.
; 4. 레이블은 snake_case입니다.
; 5. 레이블은 접두사 "lb_"가 붙습니다.
; 6. 모든 함수는 반환 레이블을 넣습니다. 예) lb_main_return:
; 7. 어셈블러 종속적인 기능은 최대한 사용하지 않습니다.
; 8. 주석을 최대한 자세하게 작성해야 합니다.
; 9. 반환값에 주석으로 비트 수가 적혀 있지 않다면 기본적으로 32비트입니다.
;=========================================

title Tetris_x86_assembly

.386
.model flat, stdcall
.stack 512

INCLUDE ConsoleUtil.inc
INCLUDE Game.inc

EXTRN ExitProcess@4 : PROC

.data
hello DB 'hello', 0     ; 테스트용

FRAME_TIME  EQU 33  ; 30 fps
start_time  DWORD   ?
end_time    DWORD   ?

.code

;=========================================
; 함수 이름: main
; 설명: 이 함수는 진입점 함수입니다.
;       테트리스 게임 루프를 수행합니다.
; 매개변수: 없음
; 반환값: 없음
;
; 함수 프롤로그/에필로그 없음
;=========================================
main:
    call InitConsole    ; 게임 시작 전, 콘솔 초기화
    call InitGame       ; 게임 초기화

    ; 프레임 시작 시간
    call GetTickCount@0
    mov start_time, eax
lb_game_loop:
    call IsRunningGame  ; 게임 오버인지 확인하기
    test eax, eax
    jz lb_main_return

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

lb_main_return:
    push 0
    call ExitProcess@4
END main