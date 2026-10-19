;=====================================================================
; ENTRADA.INC - Aplica las acciones del teclado al intento (RF-06, RF-07)
; Trabaja sobre: intento[], posActiva, colorSel, numPos, numColores.
;=====================================================================

;---------------------------------------------------------------------
; ProcesarAccion
;   Recibe:    AL = accion (ACC_xxx), AH = ID de simbolo si es ACC_SIMBOLO
;   Devuelve:  AL = resultado (RES_xxx de CONST.INC)
;   Destruye:  AX (AL es el resultado)
;
; Solo DECIDE y modifica el intento; no dibuja ni evalua. Quien llama
; mira el resultado: RES_CAMBIO -> redibujar, RES_INCOMPLETO -> avisar
; (sin gastar turno), RES_CONFIRMAR -> evaluar, RES_SALIR -> salir.
;---------------------------------------------------------------------
ProcesarAccion PROC
        push dx

        cmp  al, ACC_IZQ
        jne  paDer
        call MoverIzquierda
        jmp  paCambio
paDer:
        cmp  al, ACC_DER
        jne  paArriba
        call MoverDerecha
        jmp  paCambio
paArriba:
        cmp  al, ACC_ARRIBA
        jne  paAbajo
        mov  dl, 1              ; avanzar al simbolo siguiente
        call CambiarSimbolo
        jmp  paCambio
paAbajo:
        cmp  al, ACC_ABAJO
        jne  paSimbolo
        mov  dl, 0FFh           ; -1: retroceder al simbolo anterior
        call CambiarSimbolo
        jmp  paCambio
paSimbolo:
        cmp  al, ACC_SIMBOLO
        jne  paBorrar
        mov  al, ah             ; AL = ID del simbolo elegido
        call PonerSimbolo
        jmp  paCambio
paBorrar:
        cmp  al, ACC_BORRAR
        jne  paConfirmar
        call BorrarPosicion
        jmp  paCambio
paConfirmar:
        cmp  al, ACC_CONFIRMAR
        jne  paSalir
        call IntentoCompleto    ; AL = 1 si no hay casillas vacias
        or   al, al
        jz   paIncompleto
        mov  al, RES_CONFIRMAR
        jmp  paFin
paIncompleto:
        mov  al, RES_INCOMPLETO ; no se evalua y el turno NO avanza (RF-07)
        jmp  paFin
paSalir:
        cmp  al, ACC_SALIR
        jne  paNada
        mov  al, RES_SALIR
        jmp  paFin
paNada:
        mov  al, RES_NADA       ; ACC_NINGUNA: tecla invalida, se ignora
        jmp  paFin
paCambio:
        mov  al, RES_CAMBIO
paFin:
        pop  dx
        ret
ProcesarAccion ENDP

;---------------------------------------------------------------------
; MoverIzquierda
;   Recibe:    posActiva, numPos
;   Devuelve:  posActiva una casilla a la izquierda (de 0 salta a N-1)
;   Destruye:  nada
;---------------------------------------------------------------------
MoverIzquierda PROC
        push ax
        mov  al, posActiva
        or   al, al
        jnz  miRestar
        mov  al, numPos         ; estaba en 0: pasar a la ultima (N-1)
miRestar:
        dec  al
        mov  posActiva, al
        pop  ax
        ret
MoverIzquierda ENDP

;---------------------------------------------------------------------
; MoverDerecha
;   Recibe:    posActiva, numPos
;   Devuelve:  posActiva una casilla a la derecha (de N-1 salta a 0)
;   Destruye:  nada
;---------------------------------------------------------------------
MoverDerecha PROC
        push ax
        mov  al, posActiva
        inc  al
        cmp  al, numPos
        jb   mdGuardar
        xor  al, al             ; paso la ultima: volver a la primera
mdGuardar:
        mov  posActiva, al
        pop  ax
        ret
MoverDerecha ENDP

;---------------------------------------------------------------------
; CambiarSimbolo
;   Recibe:    DL = 1 (siguiente) o 0FFh (anterior); posActiva, numColores
;   Devuelve:  intento[posActiva] y colorSel con el nuevo simbolo
;              (ciclo 1..C; una casilla vacia pasa a 1 o a C)
;   Destruye:  nada
;---------------------------------------------------------------------
CambiarSimbolo PROC
        push ax
        push bx
        mov  bl, posActiva
        xor  bh, bh             ; BX = indice de la casilla activa
        mov  al, intento[bx]
        add  al, dl             ; +1 o -1 (0FFh) sobre el simbolo actual
        jz   csUltimo           ; bajo de 1 a 0: dar la vuelta hasta C
        cmp  al, numColores
        jbe  csGuardar          ; sigue dentro de 1..C
        cmp  dl, 1              ; se salio por arriba (C+1) o era 0 bajando (FFh)
        jne  csUltimo
        mov  al, 1              ; subio de C: dar la vuelta hasta 1
        jmp  csGuardar
csUltimo:
        mov  al, numColores
csGuardar:
        mov  intento[bx], al
        mov  colorSel, al
        pop  bx
        pop  ax
        ret
CambiarSimbolo ENDP

;---------------------------------------------------------------------
; PonerSimbolo
;   Recibe:    AL = ID del simbolo (1..C); posActiva, numPos
;   Devuelve:  intento[posActiva] = AL, colorSel = AL y la posicion
;              activa avanza una casilla (si no era la ultima)
;   Destruye:  nada
;---------------------------------------------------------------------
PonerSimbolo PROC
        push ax
        push bx
        mov  bl, posActiva
        xor  bh, bh
        mov  intento[bx], al
        mov  colorSel, al
        inc  bl                 ; avanzar para escribir el codigo de corrido
        cmp  bl, numPos
        jae  psFin              ; ya estaba en la ultima: se queda ahi
        mov  posActiva, bl
psFin:
        pop  bx
        pop  ax
        ret
PonerSimbolo ENDP

;---------------------------------------------------------------------
; BorrarPosicion
;   Recibe:    posActiva
;   Devuelve:  intento[posActiva] = 0 (casilla vacia)
;   Destruye:  nada
;---------------------------------------------------------------------
BorrarPosicion PROC
        push bx
        mov  bl, posActiva
        xor  bh, bh
        mov  intento[bx], 0
        pop  bx
        ret
BorrarPosicion ENDP

;---------------------------------------------------------------------
; IntentoCompleto  (RF-07)
;   Recibe:    intento[], numPos
;   Devuelve:  AL = 1 si las numPos casillas tienen simbolo; AL = 0 si
;              alguna esta vacia (valor 0)
;   Destruye:  AX
;---------------------------------------------------------------------
IntentoCompleto PROC
        push bx
        push cx
        mov  cl, numPos
        xor  ch, ch
        xor  bx, bx
icRevisar:
        cmp  intento[bx], 0
        je   icFalta
        inc  bx
        loop icRevisar
        mov  al, 1
        jmp  icFin
icFalta:
        xor  al, al
icFin:
        pop  cx
        pop  bx
        ret
IntentoCompleto ENDP