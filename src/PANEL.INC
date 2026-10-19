
;=====================================================================
; PANEL.INC - Panel lateral (zona C): paleta de fichas, intento en
; construccion, contadores y estado. Solo se redibuja lo que cambia.
;=====================================================================

;---------------------------------------------------------------------
; DibujarPaleta
;   Recibe:    numColores, colorSel
;   Devuelve:  las C fichas disponibles en 2 filas de 4, cada una con
;              su numero de tecla; la seleccionada lleva marco dorado
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarPaleta PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        xor  di, di             ; DI = ficha (0..C-1)
dpFicha:
        mov  ax, di
        and  ax, 3              ; columna 0..3
        mov  bl, 27
        mul  bl
        add  ax, PAN_X + 7      ; x de la ficha (centrada en celdas de 27 px)
        mov  cx, ax
        mov  ax, di
        shr  ax, 2              ; fila 0..1
        mov  bl, 28
        mul  bl
        add  ax, PAN_Y + 16     ; y de la ficha
        mov  bx, ax

        mov  dl, C_PANEL        ; marco "apagado" = color del panel
        mov  ax, di
        inc  al                 ; ID de esta ficha
        cmp  al, colorSel
        jne  dpMarco
        mov  dl, C_DORADO       ; marco de la ficha seleccionada
dpMarco:
        mov  ax, cx
        dec  ax
        mov  si, bx
        dec  si
        RELLENAR_RECT dx, ax, si, 18, 18
        RELLENAR_RECT C_CELDA, cx, bx, FICHA_LADO, FICHA_LADO
        mov  ax, di
        inc  al
        call DibujarFicha
        add  cx, 4              ; numero de tecla debajo de la ficha
        add  bx, 18
        mov  dl, C_TEXTO
        mov  dh, 1
        call DibujarNumero      ; AL sigue siendo el ID

        inc  di
        mov  al, numColores
        xor  ah, ah
        cmp  di, ax
        jb   dpFicha

        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarPaleta ENDP

;---------------------------------------------------------------------
; DibujarIntentoActual
;   Recibe:    intento[], posActiva, numPos, cuadroAnim
;   Devuelve:  las N casillas del intento en construccion; la activa
;              lleva el cursor animado de 2 cuadros (RG-05)
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarIntentoActual PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        xor  di, di             ; DI = casilla
diCasilla:
        mov  ax, di
        mov  bl, 21             ; 20 px de casilla + 1 de separacion
        mul  bl
        add  ax, PAN_X + 4
        mov  cx, ax             ; CX = x de la casilla
        mov  bx, PAN_Y + 86     ; BX = y de la casilla
        RELLENAR_RECT C_FILA, cx, bx, 20, 20

        mov  dl, intento[di]
        or   dl, dl
        jz   diCursor           ; casilla vacia: no hay ficha
        add  cx, 2
        add  bx, 2
        RELLENAR_RECT C_CELDA, cx, bx, FICHA_LADO, FICHA_LADO
        mov  al, dl
        call DibujarFicha
        sub  cx, 2
        sub  bx, 2
diCursor:
        mov  ax, di
        cmp  al, posActiva
        jne  diSiguiente
        mov  si, OFFSET CURSOR_0
        cmp  cuadroAnim, 0
        je   diMarcar
        mov  si, OFFSET CURSOR_1
diMarcar:
        push si
        push CURSOR_ANCHO
        push CURSOR_ALTO
        push cx
        push bx
        call DibujarSprite
diSiguiente:
        inc  di
        mov  al, numPos
        xor  ah, ah
        cmp  di, ax
        jb   diCasilla

        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarIntentoActual ENDP

;---------------------------------------------------------------------
; DibujarContadores  (RF-10)
;   Recibe:    turno, maxIntentos
;   Devuelve:  "Intento k/M" y "Quedan r" actualizados en el panel
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarContadores PROC
        push ax
        push bx
        push cx
        push dx
        push si

        RELLENAR_RECT C_PANEL, PAN_X+4, 134, 104, 22
        TEXTO tIntento, 208, 134, C_TEXTO, 1
        TEXTO tQuedan,  208, 146, C_TEXTO, 1
        TEXTO tBarraNum, 284, 134, C_TEXTO, 1

        mov  al, turno
        inc  al                 ; intento en curso = turno + 1
        cmp  al, maxIntentos
        jbe  dcMostrar
        mov  al, maxIntentos    ; tras el ultimo intento no pasar de M
dcMostrar:
        mov  cx, 268
        mov  bx, 134
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarNumero
        mov  al, maxIntentos
        mov  cx, 292
        call DibujarNumero
        call IntentosRestantes  ; AL = maxIntentos - turno
        mov  cx, 268
        mov  bx, 146
        call DibujarNumero

        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarContadores ENDP