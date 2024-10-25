;=========================================
; 코딩 컨벤션
; 1. 자료형은 UPPER_CASE입니다.
; 2. main 함수를 제외한 모든 public 함수는 PascalCase입니다.
; 3. main 함수를 제외한 모든 private 함수는 cameCase입니다.
; 4. 레이블은 snake_case입니다.
; 5. 레이블은 접두사 "lb_"가 붙습니다.
; 6. 어셈블러에서 지원되는 문법은 최대한 사용하지 않습니다.
;=========================================

title Tetris_x86_assembly

.386
.model flat, stdcall
.stack 512

INCLUDE ConsoleUtil.inc
INCLUDE Game.inc

EXTRN ExitProcess@4 : PROC

.data
hello DB 'hello', 0

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

    xor ebx, ebx

lb_game_loop:
    inc ebx
    call IsRunningGame  ; 게임 오버인지 확인하기
    test eax, eax
    jnz lb_return

    call UpdateGame     ; 게임 로직 업데이트
    call DrawGame       ; 게임 그리기

    jmp lb_game_loop

lb_main_return:
    push 0
    call ExitProcess@4
END main