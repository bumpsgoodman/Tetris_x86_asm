.code

;=========================================
; 함수 이름: DrawStatusSection
; 설명: 이 함수는 STATUS 섹션을 그리는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
DrawStatusSection:
    ; 프롤로그
    push ebp
    mov ebp, esp

lb_draw_status_section_return:
    ; 에필로그
    mov esp, ebp
    pop ebp
    ret

;=========================================
; 함수 이름: drawSection
; 설명: 이 함수는 섹션을 그리는 함수입니다.
; 매개변수: 없음
; 반환값: 없음
;=========================================
drawSection:
    ; 프롤로그
    push ebp
    mov ebp, esp

lb_draw_section_return:
    ; 에필로그
    mov esp, ebp
    pop ebp
    ret