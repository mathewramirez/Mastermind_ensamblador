;=====================================================================
; CICLO.INC - Ciclo de la partida (Fase 4): registrar el intento,
; avanzar el turno y detectar victoria o derrota (RF-09, RF-10, RF-11)
; Trabaja sobre: intento[], historial[], histExactos[], histParciales[],
;                turno, estado, numPos, maxIntentos.
;=====================================================================

;---------------------------------------------------------------------
; LimpiarIntento
;   Recibe:    numPos
;   Devuelve:  intento[] en 0 (casillas vacias), posActiva = 0
;   Destruye:  nada
;---------------------------------------------------------------------
LimpiarIntento PROC
        push bx
        push cx
        mov  cl, numPos
        xor  ch, ch
        xor  bx, bx
liBorrar:
        mov  intento[bx], 0
        inc  bx
        loop liBorrar
        mov  posActiva, 0
        pop  cx
        pop  bx
        ret
LimpiarIntento ENDP

;---------------------------------------------------------------------
; GuardarIntento
;   Recibe:    AL = exactos, AH = parciales; intento[]; turno
;   Devuelve:  la fila 'turno' del historial con los simbolos del
;              intento y sus dos contadores (NO avanza el turno)
;   Destruye:  nada
; La fila t empieza en historial[t*MAX_N]; el desplazamiento dentro de
; la fila se suma con direccionamiento base-indice [BX+SI] (RT-05).
;---------------------------------------------------------------------
GuardarIntento PROC
        push ax
        push bx
        push cx
        push dx
        push si

        mov  dx, ax             ; DL = exactos, DH = parciales (MUL destruye AX)
        mov  al, turno
        mov  bl, MAX_N
        mul  bl                 ; AX = turno * MAX_N = inicio de la fila
        mov  bx, ax
        xor  si, si
        mov  cl, numPos
        xor  ch, ch
gdCopiar:
        mov  al, intento[si]
        mov  historial[bx+si], al
        inc  si
        loop gdCopiar

        mov  bl, turno
        xor  bh, bh
        mov  histExactos[bx], dl
        mov  histParciales[bx], dh

        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
GuardarIntento ENDP

;---------------------------------------------------------------------
; ActualizarEstado
;   Recibe:    AL = exactos del ultimo intento; turno (ya avanzado)
;   Devuelve:  estado = VICTORIA si exactos == numPos;
;              si no, DERROTA si turno ya alcanzo maxIntentos;
;              si no, sigue JUGANDO
;   Destruye:  nada
; La victoria se revisa primero: acertar en el ultimo intento gana.
;---------------------------------------------------------------------
ActualizarEstado PROC
        push ax
        cmp  al, numPos
        jne  aeNoGano
        mov  estado, ESTADO_VICTORIA
        jmp  aeFin
aeNoGano:
        mov  al, turno
        cmp  al, maxIntentos
        jb   aeFin              ; aun quedan intentos
        mov  estado, ESTADO_DERROTA
aeFin:
        pop  ax
        ret
ActualizarEstado ENDP

;---------------------------------------------------------------------
; ConfirmarIntento  (RF-08, RF-09, RF-10, RF-11)
;   Recibe:    intento[] COMPLETO (quien llama ya lo verifico con
;              ProcesarAccion -> RES_CONFIRMAR)
;   Devuelve:  AL = estado (ESTADO_JUGANDO / VICTORIA / DERROTA)
;              AH = exactos y DL = parciales del intento evaluado
;              (para que la pantalla los muestre)
;   Destruye:  AX, DL
; Orden: evaluar -> guardar en el historial -> avanzar turno ->
; actualizar estado -> dejar el intento vacio para el siguiente turno.
;---------------------------------------------------------------------
ConfirmarIntento PROC
        call EvaluarIntento     ; AL = exactos, AH = parciales
        call GuardarIntento
        inc  turno
        push ax
        call ActualizarEstado   ; usa AL = exactos
        call LimpiarIntento
        pop  dx                 ; DL = exactos, DH = parciales
        mov  ah, dl             ; AH = exactos
        mov  dl, dh             ; DL = parciales
        mov  al, estado
        ret
ConfirmarIntento ENDP

;---------------------------------------------------------------------
; IntentosRestantes
;   Recibe:    turno, maxIntentos
;   Devuelve:  AL = maxIntentos - turno
;   Destruye:  AL
;---------------------------------------------------------------------
IntentosRestantes PROC
        mov  al, maxIntentos
        sub  al, turno
        ret
IntentosRestantes ENDP