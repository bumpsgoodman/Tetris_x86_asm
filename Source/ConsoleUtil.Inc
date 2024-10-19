EXTRN GetStdHandle@4 : PROC
EXTRN WriteFile@20 : PROC

EXTRN GetConsoleMode@8 : PROC
EXTRN SetConsoleMode@8 : PROC

INCLUDE AsciiUtil.inc

.data
STD_OUT DWORD ?
STD_IN DWORD ?

SEQ_HIDE_CURSOR BYTE 1Bh, '[?25l', 0  ; 커서 숨김 시퀀스

.code
InitConsole PROC
    LOCAL consoleMode:DWORD

    ; STD_OUTPUT 핸들 얻기
    push -11
    call GetStdHandle@4
    mov STD_OUT, eax

    ; 콘솔 모드 얻기
    lea eax, consoleMode
    push eax
    push STD_OUT
    call GetConsoleMode@8

    ; VT 모드 켜기
    or consoleMode, 4   ; 4 = ENABLE_VIRTUAL_TERMINAL_PROCESSING
    push consoleMode
    push STD_OUT
    call SetConsoleMode@8

    ; 커서 없애기
    push LENGTHOF SEQ_HIDE_CURSOR
    push OFFSET SEQ_HIDE_CURSOR
    call PrintConsoleMsg

    ret
InitConsole ENDP

PrintConsoleMsg PROC msg:PTR BYTE, msgLen:DWORD
    push 0   
    push 0
    push msgLen
    push msg
    push STD_OUT
    call WriteFile@20

    ret
PrintConsoleMsg ENDP

; 지정된 [r/g/b]로 텍스트 색상 변경
; r - 0 ~ 255
; g - 0 ~ 255
; b - 0 ~ 255
; 리턴값 - 없음
SetConsoleTextColor PROC r:BYTE, g:BYTE, b:BYTE
    ; 텍스트 색상 지정 시퀀스
    ; ESC[38;2;r;g;bm
    ; 기본 11글자 + rgb
    LOCAL sequence[21]:BYTE
    LOCAL sequenceLen:DWORD

    push edi
    push ebx

    mov sequenceLen, 11

    lea edi, sequence
    mov BYTE PTR [edi], 1Bh
    mov BYTE PTR [edi + 1], '['
    mov BYTE PTR [edi + 2], '3'
    mov BYTE PTR [edi + 3], '8'
    mov BYTE PTR [edi + 4], ';'
    mov BYTE PTR [edi + 5], '2'
    mov BYTE PTR [edi + 6], ';'
    add edi, 7

    ; red 문자로 변경
    xor ebx, ebx
    mov bl, r
    push edi
    push ebx
    call IntToAscii
    add edi, eax
    add sequenceLen, eax

    mov BYTE PTR [edi], ';'
    inc edi

    ; green 문자로 변경
    xor ebx, ebx
    mov bl, g
    push edi
    push ebx
    call IntToAscii
    add edi, eax
    add sequenceLen, eax

    mov BYTE PTR [edi], ';'
    inc edi

    ; blue 문자로 변경
    xor ebx, ebx
    mov bl, b
    push edi
    push ebx
    call IntToAscii
    add edi, eax
    add sequenceLen, eax

    mov BYTE PTR [edi], 'm'
    mov BYTE PTR [edi + 1], 0

    lea edi, sequence
    push sequenceLen
    push edi
    call PrintConsoleMsg

    pop ebx
    pop edi

    ret
SetConsoleTextColor ENDP

; 지정된 [r/g/b]로 배경 색상 변경
; r - 0 ~ 255
; g - 0 ~ 255
; b - 0 ~ 255
; 리턴값 - 없음
SetConsoleBackColor PROC r:BYTE, g:BYTE, b:BYTE
    ; 백그라운드 색상 지정 시퀀스
    ; ESC[48;2;r;g;bm
    LOCAL sequence[21]:BYTE
    LOCAL sequenceLen:DWORD

    push edi
    push ebx

    mov sequenceLen, 11

    lea edi, sequence
    mov BYTE PTR [edi], 1Bh
    mov BYTE PTR [edi + 1], '['
    mov BYTE PTR [edi + 2], '4'
    mov BYTE PTR [edi + 3], '8'
    mov BYTE PTR [edi + 4], ';'
    mov BYTE PTR [edi + 5], '2'
    mov BYTE PTR [edi + 6], ';'
    add edi, 7

    ; red 문자로 변경
    xor ebx, ebx
    mov bl, r
    push edi
    push ebx
    call IntToAscii
    add edi, eax
    add sequenceLen, eax

    mov BYTE PTR [edi], ';'
    inc edi

    ; green 문자로 변경
    xor ebx, ebx
    mov bl, g
    push edi
    push ebx
    call IntToAscii
    add edi, eax
    add sequenceLen, eax

    mov BYTE PTR [edi], ';'
    inc edi

    ; blue 문자로 변경
    xor ebx, ebx
    mov bl, b
    push edi
    push ebx
    call IntToAscii
    add edi, eax
    add sequenceLen, eax

    mov BYTE PTR [edi], 'm'
    mov BYTE PTR [edi + 1], 0

    lea edi, sequence
    push sequenceLen
    push edi
    call PrintConsoleMsg

    pop ebx
    pop edi

    ret
SetConsoleBackColor ENDP

; [x, y]로 커서 위치 변경
; 반환값 - 없음
; 0 < x < 100
; 0 < y < 100
SetConsoleXY PROC x:DWORD, y:DWORD
    LOCAL sequence[9]:BYTE
    LOCAL sequenceLen:DWORD
    ; 커서 위치 지정 시퀀스
    ; ESC[y;xH

    push edi
    push ebx

    mov ebx, x
    mov ecx, y

    mov sequenceLen, 5

    lea edi, sequence
    mov BYTE PTR [edi], 1Bh
    inc edi
    mov BYTE PTR [edi], '['
    inc edi

    ; y 좌표 문자로 변경
    push edi
    push ecx
    call IntToAscii
    add edi, eax
    add sequenceLen, eax

    mov BYTE PTR [edi], ';'
    inc edi

    ; x 좌표 문자로 변경
    push edi
    push ebx
    call IntToAscii
    add edi, eax
    add sequenceLen, eax

    mov BYTE PTR [edi], 'H'
    inc edi
    mov BYTE PTR [edi], 0

    ; 커서 이동
    lea edi, sequence
    push sequenceLen
    push edi
    call PrintConsoleMsg

    pop ebx
    pop edi

    ret
SetConsoleXY ENDP