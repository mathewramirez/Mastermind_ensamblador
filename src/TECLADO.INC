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
;---------------------------------------------------------------------
EsperarTecla PROC
        mov  ah, 00h
        int  16h
        ret
EsperarTecla ENDP
