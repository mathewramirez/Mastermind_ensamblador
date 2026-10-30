
;=====================================================================
; TABLERO.INC - Tablero de intentos (zona B, RF-09): una fila por
; intento con su numero, sus fichas y la retroalimentacion.
; Cada fila: [numero 16 px][fichas N*paso][iconos exacto/parcial]
; Los iconos se reparten en 2 lineas de 8 px (cabe en las filas de 16).
;=====================================================================

;---------------------------------------------------------------------
; CargarGeometria
;   Recibe:    nivel
;   Devuelve:  geomFilas, geomAlto, geomAncho, geomPaso y geomFb
;              copiados de tablaGeom
;   Destruye:  nada
;---------------------------------------------------------------------
CargarGeometria PROC
        push ax
        push bx
        mov  al, nivel
        mov  bl, 5              ; 5 bytes por nivel
        mul  bl
        mov  bx, ax
        mov  al, tablaGeom[bx]
        mov  geomFilas, al
        mov  al, tablaGeom[bx+1]
        mov  geomAlto, al
        mov  al, tablaGeom[bx+2]
        mov  geomAncho, al
        mov  al, tablaGeom[bx+3]
        mov  geomPaso, al
        mov  al, tablaGeom[bx+4]
        mov  geomFb, al
        pop  bx
        pop  ax
        ret
CargarGeometria ENDP

;---------------------------------------------------------------------
; ObtenerPosFila
;   Recibe:    AL = numero de intento (0 = el primero)
;   Devuelve:  filaX, filaY = esquina de las fichas de esa fila.
;              Intento t -> columna t / filas, fila t mod filas.
;   Destruye:  nada
;---------------------------------------------------------------------
ObtenerPosFila PROC
        push ax
        push bx
        push cx
        xor  ah, ah
        div  geomFilas          ; AL = columna, AH = fila dentro de la columna
        mov  cx, ax             ; CL = columna, CH = fila
        mov  al, cl
        mul  geomAncho          ; AX = columna * ancho de columna
        add  ax, TAB_X + 2
        mov  filaX, ax
        mov  al, ch
        mul  geomAlto           ; AX = fila * alto de fila
        add  ax, TAB_Y + 2
        mov  bx, ax
        mov  al, geomAlto
        sub  al, FICHA_LADO
        shr  al, 1              ; margen para centrar la ficha en filas altas
        xor  ah, ah
        add  bx, ax
        mov  filaY, bx
        pop  cx
        pop  bx
        pop  ax
        ret
ObtenerPosFila ENDP

;---------------------------------------------------------------------
; DibujarFicha
;   Recibe:    AL = ID del simbolo (1..8), CX = x, BX = y
;   Devuelve:  el sprite de esa ficha (16x16) en (x, y)
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarFicha PROC
        push ax
        push si
        xor  ah, ah
        dec  ax
        shl  ax, 1              ; indice (ID-1)*2 en tablaFichas
        mov  si, ax
        push tablaFichas[si]
        push FICHA_LADO
        push FICHA_LADO
        push cx
        push bx
        call DibujarSprite
        pop  si
        pop  ax
        ret
DibujarFicha ENDP

;---------------------------------------------------------------------
; DibujarFichaSobre
;   Recibe:    AL = ID del simbolo, CX = x, BX = y, DL = color de fondo
;   Devuelve:  la ficha con su fondo claro en UNA sola pasada (la ficha
;              completa de 16x16 queda pintada, sin borrar antes)
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarFichaSobre PROC
        call DibujarFicha
        ret
DibujarFichaSobre ENDP

;---------------------------------------------------------------------
; DibujarFilaVacia
;   Recibe:    AL = numero de intento (0 = el primero)
;   Devuelve:  el numero del intento y sus casillas vacias
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarFilaVacia PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        xor  ah, ah
        mov  di, ax             ; DI = numero de intento
        call ObtenerPosFila
        mov  ax, di
        inc  al                 ; se muestra desde 1
        mov  cx, filaX
        mov  bx, filaY
        add  bx, 4              ; digitos de 8 px centrados en la fila de 16
        mov  dl, C_TEXTO
        mov  dh, 1
        call DibujarNumero

        xor  si, si             ; SI = casilla (0..N-1)
dvCasilla:
        mov  ax, si
        mul  geomPaso           ; AX = casilla * paso
        add  ax, filaX
        add  ax, 17             ; 16 del numero + 1 de margen
        mov  bx, filaY
        inc  bx
        RELLENAR_RECT C_FILA, ax, bx, 14, 14
        inc  si
        mov  al, numPos
        xor  ah, ah
        cmp  si, ax
        jb   dvCasilla

        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarFilaVacia ENDP

;---------------------------------------------------------------------
; DibujarIconoRetro
;   Recibe:    AL = posicion del icono dentro de la retroalimentacion
;              (0 es el primero), DL = 0 exacto / 1 parcial;
;              filaX, filaY de la fila actual
;   Devuelve:  el icono de 8x8 dibujado a la derecha de las fichas
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarIconoRetro PROC
        push ax
        push bx
        push cx
        push si

        mov  cl, numPos
        inc  cl
        shr  cl, 1              ; CL = iconos por linea (2 con N=4, 3 con N=5)
        xor  ah, ah
        div  cl                 ; AL = linea, AH = columna
        mov  bl, al             ; BL = linea
        mov  al, ah
        mul  geomFb             ; AX = columna * paso de iconos
        mov  si, ax
        mov  al, numPos
        mul  geomPaso           ; AX = ancho ocupado por las fichas
        add  si, ax
        add  si, filaX
        add  si, 17             ; SI = x del icono (despues del numero y fichas)
        mov  al, bl
        mov  cl, 8
        mul  cl                 ; AX = linea * 8
        add  ax, filaY          ; AX = y del icono
        mov  bx, OFFSET EXACTO_0
        or   dl, dl
        jz   diDibujar
        mov  bx, OFFSET PARCIAL_0
diDibujar:
        push bx
        push EXACTO_ANCHO
        push EXACTO_ALTO
        push si
        push ax
        call DibujarSprite

        pop  si
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarIconoRetro ENDP

;---------------------------------------------------------------------
; DibujarRetro
;   Recibe:    AL = numero de intento; filaX, filaY de su fila
;   Devuelve:  histExactos[t] iconos de exacto y luego histParciales[t]
;              iconos de parcial (RF-08, RF-09)
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarRetro PROC
        push ax
        push bx
        push cx
        push dx

        xor  ah, ah
        mov  bx, ax
        mov  cl, histExactos[bx]
        mov  ch, histParciales[bx]
        xor  bl, bl             ; BL = posicion del proximo icono
        xor  dl, dl             ; DL = tipo: exacto
drExactos:
        or   cl, cl
        jz   drParciales
        mov  al, bl
        call DibujarIconoRetro
        inc  bl
        dec  cl
        jmp  drExactos
drParciales:
        mov  dl, 1              ; tipo: parcial
drOtroParcial:
        or   ch, ch
        jz   drFin
        mov  al, bl
        call DibujarIconoRetro
        inc  bl
        dec  ch
        jmp  drOtroParcial
drFin:
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarRetro ENDP

;---------------------------------------------------------------------
; DibujarFilaHistorial
;   Recibe:    AL = numero de un intento ya evaluado
;   Devuelve:  sus fichas (sobre fondo claro) y su retroalimentacion
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarFilaHistorial PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        xor  ah, ah
        mov  di, ax             ; DI = numero de intento
        call ObtenerPosFila
        xor  si, si             ; SI = casilla
dhFicha:
        mov  ax, di
        mov  bl, MAX_N
        mul  bl
        add  ax, si             ; AX = t*MAX_N + casilla (fila del historial)
        mov  bx, ax
        mov  dl, historial[bx]  ; DL = ID del simbolo
        mov  ax, si
        mul  geomPaso
        add  ax, filaX
        add  ax, 16
        mov  cx, ax
        mov  bx, filaY
        mov  al, dl             ; AL = ID del simbolo
        mov  dl, C_CELDA        ; fondo claro tras la ficha
        call DibujarFichaSobre
        inc  si
        mov  al, numPos
        xor  ah, ah
        cmp  si, ax
        jb   dhFicha

        mov  ax, di
        call DibujarRetro

        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarFilaHistorial ENDP

;---------------------------------------------------------------------
; DibujarTablero
;   Recibe:    maxIntentos, turno, historial[]
;   Devuelve:  todas las filas: vacias las pendientes, completas las
;              ya jugadas (RF-09: todo el historial visible a la vez)
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarTablero PROC
        push ax
        push cx
        xor  cl, cl             ; CL = intento que se dibuja
tbFila:
        mov  al, cl
        call DibujarFilaVacia
        cmp  cl, turno
        jae  tbSiguiente
        mov  al, cl
        call DibujarFilaHistorial
tbSiguiente:
        inc  cl
        cmp  cl, maxIntentos
        jb   tbFila
        pop  cx
        pop  ax
        ret
DibujarTablero ENDP