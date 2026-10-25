;=====================================================================
; TECLADO.INC - Lectura de teclado por BIOS (INT 16h)
;=====================================================================

;---------------------------------------------------------------------
; EsperarTecla
;   Recibe:    nada
;   Devuelve:  AH = codigo de exploracion (scan code)
;              AL = codigo ASCII (0 si es tecla extendida: flechas, F1..)
;   Destruye:  AX (es el valor de retorno)
;
; INT 16h AH=00h BLOQUEA hasta que haya una tecla en el buffer.
; Antes de esperar, si hay una pantalla nueva con fundido pendiente,
; la hace aparecer (ver FADE.INC).
;---------------------------------------------------------------------
EsperarTecla PROC
        call AplicarFundidoPendiente    ; si hay pantalla nueva, que aparezca
        mov  ah, 00h
        int  16h
        ret
EsperarTecla ENDP

;---------------------------------------------------------------------
; HayTecla
;   Recibe:    nada
;   Devuelve:  ZF = 0 si hay una tecla esperando (AX = esa tecla, SIN
;              sacarla del buffer); ZF = 1 si no hay ninguna
;   Destruye:  AX
;
; INT 16h AH=01h NO bloquea. Sirve para animar mientras se espera:
; el programa sigue dibujando y solo lee la tecla cuando existe.
; El fundido pendiente se aplica ANTES de la interrupcion para no
; alterar el ZF que devuelve.
;---------------------------------------------------------------------
HayTecla PROC
        call AplicarFundidoPendiente    ; antes de INT 16h para no alterar ZF
        mov  ah, 01h
        int  16h
        ret
HayTecla ENDP

;---------------------------------------------------------------------
; VaciarTeclado
;   Recibe:    nada
;   Devuelve:  buffer del teclado vacio (descarta teclas pendientes)
;   Destruye:  nada
;
; Evita que una tecla pulsada en una pantalla anterior se aplique
; sola en la siguiente.
;---------------------------------------------------------------------
VaciarTeclado PROC
        push ax
vtOtra:
        call HayTecla
        jz   vtFin              ; ZF=1: ya no hay teclas
        call EsperarTecla       ; sacarla del buffer y descartarla
        jmp  vtOtra
vtFin:
        pop  ax
        ret
VaciarTeclado ENDP

;---------------------------------------------------------------------
; TraducirTecla
;   Recibe:    AH = scan code, AL = ASCII (lo que devuelve EsperarTecla)
;              numColores (para saber hasta que digito es valido)
;   Devuelve:  AL = accion (ACC_xxx de CONST.INC)
;              AH = ID del simbolo si AL = ACC_SIMBOLO; si no, 0
;   Destruye:  AX (es el valor de retorno)
;
; Clave del RF-06: si AL = 0 (o E0h) la tecla es EXTENDIDA y su ASCII
; no significa nada; hay que identificarla por el scan code de AH.
; Si AL <> 0 es una tecla normal y se identifica por su ASCII.
; Cualquier otra tecla devuelve ACC_NINGUNA y el juego la ignora.
;---------------------------------------------------------------------
TraducirTecla PROC
        cmp  al, 0
        je   ttExtendida
        cmp  al, 0E0h
        je   ttExtendida

        ; ---- teclas normales: se mira el ASCII (AL) ----
        cmp  al, ASC_ENTER
        jne  ttNoEnter
        mov  ax, ACC_CONFIRMAR  ; AH = 0, AL = accion
        ret
ttNoEnter:
        cmp  al, ASC_RETROCESO
        jne  ttNoBorrar
        mov  ax, ACC_BORRAR
        ret
ttNoBorrar:
        cmp  al, ASC_ESC
        jne  ttNoEsc
        mov  ax, ACC_SALIR
        ret
ttNoEsc:
        ; digitos '1'..'0'+C: el digito es directamente el ID del simbolo
        cmp  al, '1'
        jb   ttNada
        sub  al, '0'            ; AL = valor del digito (o algo grande si no era digito)
        cmp  al, numColores
        ja   ttNada             ; fuera de 1..C en este nivel
        mov  ah, al             ; AH = ID del simbolo
        mov  al, ACC_SIMBOLO
        ret

        ; ---- teclas extendidas: se mira el scan code (AH) ----
ttExtendida:
        cmp  ah, SCAN_IZQ
        jne  ttNoIzq
        mov  ax, ACC_IZQ
        ret
ttNoIzq:
        cmp  ah, SCAN_DER
        jne  ttNoDer
        mov  ax, ACC_DER
        ret
ttNoDer:
        cmp  ah, SCAN_ARRIBA
        jne  ttNoArriba
        mov  ax, ACC_ARRIBA
        ret
ttNoArriba:
        cmp  ah, SCAN_ABAJO
        jne  ttNada
        mov  ax, ACC_ABAJO
        ret
ttNada:
        mov  ax, ACC_NINGUNA
        ret
TraducirTecla ENDP