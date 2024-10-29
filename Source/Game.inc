.data

blocksBundle

INCLUDE GameDraw.inc

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