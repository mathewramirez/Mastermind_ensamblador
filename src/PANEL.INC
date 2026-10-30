
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
        ; marco de 1 px alrededor de la ficha (solo el anillo): la ficha de
        ; 16x16 se pinta encima de una sola vez, asi no hay un instante en blanco
        mov  ax, cx
        dec  ax                 ; AX = x-1
        mov  si, bx
        dec  si                 ; SI = y-1
        RELLENAR_RECT dx, ax, si, 18, 1     ; borde superior
        add  si, 17
        RELLENAR_RECT dx, ax, si, 18, 1     ; borde inferior
        mov  si, bx
        RELLENAR_RECT dx, ax, si, 1, 16     ; borde izquierdo
        add  ax, 17
        RELLENAR_RECT dx, ax, si, 1, 16     ; borde derecho
        mov  ax, di
        inc  al
        mov  dl, C_CELDA
        call DibujarFichaSobre
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
; ComponerSprite
;   Recibe:    SI = sprite, BL = ancho, BH = alto,
;              DI = posicion destino dentro de casBuf (filas de 20 bytes)
;   Devuelve:  el sprite copiado en casBuf; los pixeles de indice 0
;              (COLOR_TRANSP) NO se copian, asi se ve lo que ya habia debajo
;   Destruye:  nada
;---------------------------------------------------------------------
ComponerSprite PROC
        push ax
        push cx
        push dx
        push si
        push di
        mov  dl, bh             ; DL = filas que faltan
csFila:
        mov  cl, bl
        xor  ch, ch             ; CX = columnas de esta fila
        push di
csPixel:
        lodsb
        cmp  al, COLOR_TRANSP
        je   csSalta            ; transparente: dejar el pixel que ya hay
        mov  [di], al
csSalta:
        inc  di
        loop csPixel
        pop  di
        add  di, CASILLA_LADO   ; bajar una fila dentro del buffer
        dec  dl
        jnz  csFila
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  ax
        ret
ComponerSprite ENDP

;---------------------------------------------------------------------
; DibujarCasilla
;   Recibe:    AL = casilla (0..N-1) del intento en construccion
;   Devuelve:  esa casilla redibujada; si es la activa lleva el cursor
;              animado de 2 cuadros (RG-05).
;   Destruye:  nada
;   Anti-parpadeo (RG-08) con transparencia real: la casilla de 20x20 se
;   arma primero en memoria (casBuf): fondo, cursor y ficha, con el
;   indice 0 sin copiar. Despues se vuelca a la pantalla de una sola vez,
;   asi cada pixel se escribe una unica vez y nunca se ve la casilla
;   "a medias" aunque el cursor se redibuje cada cuadro.
;---------------------------------------------------------------------
DibujarCasilla PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di
        push es

        mov  dl, al             ; DL = casilla
        xor  dh, dh
        mov  si, dx
        mov  dh, intento[si]    ; DH = ficha puesta (0 = vacia)
        mov  bl, 21             ; 20 px de casilla + 1 de separacion
        mul  bl
        add  ax, PAN_X + 4
        push ax                 ; x de la casilla
        mov  ax, PAN_Y + 86
        push ax                 ; y de la casilla

        ; 1) fondo de la casilla en el buffer
        push ds
        pop  es
        mov  di, OFFSET casBuf
        mov  al, C_FILA
        mov  cx, CASILLA_LADO * CASILLA_LADO
        rep  stosb

        ; 2) cursor animado, solo en la casilla activa
        cmp  dl, posActiva
        jne  dcSinCursor
        mov  si, OFFSET CURSOR_0
        cmp  cuadroAnim, 0
        je   dcCursor
        mov  si, OFFSET CURSOR_1
dcCursor:
        mov  bl, CURSOR_ANCHO
        mov  bh, CURSOR_ALTO
        mov  di, OFFSET casBuf
        call ComponerSprite
dcSinCursor:

        ; 3) ficha encima del cursor, centrada (margen de 2 px)
        or   dh, dh
        jz   dcVolcar
        mov  al, dh
        xor  ah, ah
        dec  ax
        shl  ax, 1
        mov  si, ax
        mov  si, tablaFichas[si]
        mov  bl, FICHA_LADO
        mov  bh, FICHA_LADO
        mov  di, OFFSET casBuf + 2 * CASILLA_LADO + 2
        call ComponerSprite

        ; 4) volcar el buffer a la pantalla en una sola pasada
dcVolcar:
        mov  ax, VIDEO_SEG
        mov  es, ax
        pop  bx                 ; BX = y
        pop  cx                 ; CX = x
        mov  di, bx
        shl  di, 8
        shl  bx, 6
        add  di, bx
        add  di, cx             ; DI = y*320 + x
        mov  si, OFFSET casBuf
        mov  dl, CASILLA_LADO   ; filas
dcVolFila:
        mov  cx, CASILLA_LADO
        rep  movsb
        add  di, ANCHO_PANT - CASILLA_LADO
        dec  dl
        jnz  dcVolFila

        pop  es
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarCasilla ENDP

;---------------------------------------------------------------------
; DibujarIntentoActual
;   Recibe:    intento[], posActiva, numPos, cuadroAnim
;   Devuelve:  las N casillas del intento en construccion
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarIntentoActual PROC
        push ax
        push cx
        xor  cl, cl             ; CL = casilla
diCasilla:
        mov  al, cl
        call DibujarCasilla
        inc  cl
        cmp  cl, numPos
        jb   diCasilla
        pop  cx
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

        ; Sin borrar antes (RG-08): los textos y numeros se dibujan con fondo
        ; C_PANEL, asi cada pixel se escribe una sola vez y no hay instante en blanco.
        mov  ax, C_PANEL
        mov  fondoTexto, ax
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
        mov  fondoTexto, 0FFFFh ; volver al texto sin fondo

        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarContadores ENDP