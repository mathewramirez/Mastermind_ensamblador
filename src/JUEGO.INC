;=====================================================================
; JUEGO.INC - Logica del juego: niveles, reinicio y evaluacion
;=====================================================================

;---------------------------------------------------------------------
; ConfigurarNivel
;   Recibe:    nivel (0=facil, 1=normal, 2=dificil)
;   Devuelve:  numPos, numColores, repeticion y maxIntentos copiados
;              de tablaNiveles
;   Destruye:  nada
;---------------------------------------------------------------------
ConfigurarNivel PROC
        push ax
        push bx
        mov  bl, nivel
        xor  bh, bh
        shl  bx, 2              ; BX = nivel*4: cada fila de la tabla mide 4 bytes
        mov  al, tablaNiveles[bx]
        mov  numPos, al
        mov  al, tablaNiveles[bx+1]
        mov  numColores, al
        mov  al, tablaNiveles[bx+2]
        mov  repeticion, al
        mov  al, tablaNiveles[bx+3]
        mov  maxIntentos, al
        pop  bx
        pop  ax
        ret
ConfigurarNivel ENDP

;---------------------------------------------------------------------
; IniciarPartida
;   Recibe:    nivel
;   Devuelve:  configuracion cargada, arreglos en 0, turno = 0,
;              estado = jugando y un secreto nuevo
;   Destruye:  nada
;---------------------------------------------------------------------
IniciarPartida PROC
        push ax
        push cx
        push di
        push es

        call ConfigurarNivel
        push ds
        pop  es                 ; STOSB escribe en ES:DI y los arreglos estan en DS
        cld
        xor  al, al             ; AL = 0 = valor de relleno
        LIMPIAR_ARREGLO secreto,       MAX_N
        LIMPIAR_ARREGLO intento,       MAX_N
        LIMPIAR_ARREGLO historial,     MAX_INTENTOS*MAX_N
        LIMPIAR_ARREGLO histExactos,   MAX_INTENTOS
        LIMPIAR_ARREGLO histParciales, MAX_INTENTOS

        mov  turno, 0
        mov  posActiva, 0
        mov  colorSel, 1
        mov  estado, ESTADO_JUGANDO
        call GenerarSecreto

        pop  es
        pop  di
        pop  cx
        pop  ax
        ret
IniciarPartida ENDP

;---------------------------------------------------------------------
; EvaluarIntento  (RF-08)
;   Recibe:    secreto[] e intento[] con numPos IDs (1..C) cada uno
;   Devuelve:  AL = exactos  (simbolo correcto en su posicion)
;              AH = parciales (simbolo correcto en otra posicion)
;   Destruye:  AX; no modifica secreto ni intento (trabaja con copias)
;
; Equivale al calculo de "toros y vacas" del enunciado:
;   exacto = toro, parcial = vaca.
; 1) Exactos: misma posicion y mismo simbolo. Ambas casillas se ponen
;    en 0 para que esa ficha no se cuente otra vez.
; 2) Parciales: cada ficha restante del intento busca UNA igual en lo
;    que queda del secreto; al encontrarla se anula en el secreto.
;    Asi, si el secreto tiene dos fichas iguales y el intento tres,
;    solo dos pueden producir aciertos.
;---------------------------------------------------------------------
EvaluarIntento PROC
        push bx
        push cx
        push dx
        push si
        push di
        push es

        push ds
        pop  es                 ; MOVSB copia DS:SI -> ES:DI (todo es nuestro segmento)
        cld
        mov  bl, numPos
        xor  bh, bh             ; BX = N (tambien es el limite del ciclo interno)

        mov  cx, bx
        mov  si, OFFSET secreto
        mov  di, OFFSET copiaS
        rep  movsb              ; copiaS = secreto
        mov  cx, bx
        mov  si, OFFSET intento
        mov  di, OFFSET copiaI
        rep  movsb              ; copiaI = intento

        xor  dx, dx             ; DL = exactos, DH = parciales

        ; ---- 1) exactos ----
        mov  cx, bx
        xor  si, si
evExactos:
        mov  al, copiaS[si]
        or   al, al
        jz   evNoExacto         ; casilla vacia: nada que comparar
        cmp  al, copiaI[si]
        jne  evNoExacto
        inc  dl
        mov  copiaS[si], 0
        mov  copiaI[si], 0
evNoExacto:
        inc  si
        loop evExactos

        ; ---- 2) parciales ----
        mov  cx, bx
        xor  si, si
evAfuera:
        mov  al, copiaI[si]
        or   al, al
        jz   evSigI             ; ya fue exacto (o esta vacia)
        xor  di, di
evAdentro:
        cmp  al, copiaS[di]
        jne  evNoParcial
        inc  dh
        mov  copiaS[di], 0      ; esa ficha del secreto ya no puede contarse otra vez
        jmp  evSigI
evNoParcial:
        inc  di
        cmp  di, bx
        jb   evAdentro
evSigI:
        inc  si
        loop evAfuera

        mov  ax, dx             ; AL = exactos (DL), AH = parciales (DH)

        pop  es
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
EvaluarIntento ENDP