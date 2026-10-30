;=====================================================================
; TEXTO.INC - Rectangulos y texto en modo 13h (Fase 5)
; El texto se dibuja pixel a pixel con la fuente de 8x8 que trae el
; BIOS. Asi se puede poner en cualquier (x,y), con color de la paleta
; propia y a escala 1, 2 o 3 (titulos grandes) sin depender del cursor
; de texto de INT 10h.
;=====================================================================

;---------------------------------------------------------------------
; RellenarRect
;   Recibe por PILA (o con la macro RELLENAR_RECT):
;       push color  [bp+12]    push x [bp+10]    push y [bp+8]
;       push ancho  [bp+6]     push alto [bp+4]
;   Devuelve:  rectangulo de color solido. Saca 10 bytes ("ret 10").
;   Destruye:  nada
;---------------------------------------------------------------------
RellenarRect PROC
        push bp
        mov  bp, sp
        push ax
        push cx
        push dx
        push di
        push es

        mov  ax, VIDEO_SEG
        mov  es, ax
        mov  di, [bp+8]         ; DI = y*320 + x, igual que DibujarSprite
        mov  ax, di
        shl  di, 8
        shl  ax, 6
        add  di, ax
        add  di, [bp+10]
        mov  al, [bp+12]        ; AL = color
        mov  dx, [bp+4]         ; DX = filas
        or   dx, dx
        jz   rrFin
rrFila:
        mov  cx, [bp+6]
        rep  stosb              ; una fila de 'ancho' pixeles
        add  di, ANCHO_PANT     ; bajar una linea y volver a la x inicial
        sub  di, [bp+6]
        dec  dx
        jnz  rrFila
rrFin:
        pop  es
        pop  di
        pop  dx
        pop  cx
        pop  ax
        pop  bp
        ret  10
RellenarRect ENDP

;---------------------------------------------------------------------
; CargarFuente
;   Recibe:    nada
;   Devuelve:  fuenteBaja* y fuenteAlta* con la direccion de la fuente
;              de 8x8 del BIOS (INT 10h AX=1130h; BH=3 -> chars 0..127,
;              BH=4 -> chars 128..255). Llamar UNA vez tras entrar a 13h.
;   Destruye:  nada
;---------------------------------------------------------------------
CargarFuente PROC
        push ax
        push bx
        push cx
        push dx
        push bp
        push es

        mov  ax, 1130h
        mov  bh, 3
        int  10h                ; devuelve ES:BP = tabla
        mov  fuenteBajaSeg, es
        mov  fuenteBajaOff, bp
        mov  ax, 1130h
        mov  bh, 4
        int  10h
        mov  fuenteAltaSeg, es
        mov  fuenteAltaOff, bp

        pop  es
        pop  bp
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
CargarFuente ENDP

;---------------------------------------------------------------------
; DibujarCaracter
;   Recibe por PILA:
;       push caracter [bp+12]   push x [bp+10]   push y [bp+8]
;       push color    [bp+6]    push escala [bp+4]   (1, 2 o 3)
;   Devuelve:  el caracter dibujado; solo se escriben los pixeles
;              "encendidos" del glifo, el fondo no se toca. Excepcion: si
;              fondoTexto vale 0..255, los pixeles apagados se pintan con ese
;              color, asi un texto que cambia (el cronometro) se reemplaza
;              en una sola pasada, sin borrar antes (RG-08).
;              Saca 10 bytes ("ret 10").
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarCaracter PROC
        push bp
        mov  bp, sp
        push ax
        push bx
        push cx
        push dx
        push si
        push di
        push es

        ; ---- 1) copiar las 8 filas del glifo desde la ROM a glifo[] ----
        mov  ax, [bp+12]
        xor  ah, ah
        mov  si, fuenteBajaOff
        mov  bx, fuenteBajaSeg
        cmp  al, 128
        jb   dcCopiar
        sub  al, 128            ; la segunda mitad vive en otra tabla
        mov  si, fuenteAltaOff
        mov  bx, fuenteAltaSeg
dcCopiar:
        shl  ax, 3              ; cada glifo ocupa 8 bytes
        add  si, ax
        mov  es, bx
        mov  cx, 8
        xor  bx, bx
dcLeer:
        mov  al, es:[si+bx]
        mov  glifo[bx], al
        inc  bx
        loop dcLeer

        ; ---- 2) DI = y*320 + x ----
        mov  di, [bp+8]
        mov  ax, di
        shl  di, 8
        shl  ax, 6
        add  di, ax
        add  di, [bp+10]
        mov  ax, VIDEO_SEG
        mov  es, ax
        mov  dl, [bp+6]         ; DL = color
        mov  dh, BYTE PTR fondoTexto ; DH = color de fondo (solo si fondoTexto <> 0FFFFh)
        xor  si, si             ; SI = fila del glifo (0..7)

        ; ---- 3) cada bit encendido = un bloque escala x escala ----
dcFila:
        mov  bx, [bp+4]         ; BX = veces que se repite la fila (escala)
dcRepetir:
        push di                 ; recordar el inicio de esta linea
        mov  al, glifo[si]
        mov  cx, 8              ; 8 bits por fila, el bit 7 es el de la izquierda
dcBit:
        test al, 80h
        jz   dcApagado
        push cx
        mov  cx, [bp+4]
dcPixel:
        mov  es:[di], dl
        inc  di
        loop dcPixel
        pop  cx
        jmp  dcSiguiente
dcApagado:
        cmp  fondoTexto, 0FFFFh
        je   dcSaltar           ; sin fondo: dejar lo que ya hay en pantalla
        push cx
        mov  cx, [bp+4]
dcFondo:
        mov  es:[di], dh        ; con fondo: pintar el pixel apagado
        inc  di
        loop dcFondo
        pop  cx
        jmp  dcSiguiente
dcSaltar:
        add  di, [bp+4]         ; saltar 'escala' pixeles sin escribir
dcSiguiente:
        shl  al, 1
        loop dcBit
        pop  di
        add  di, ANCHO_PANT     ; siguiente linea de pantalla
        dec  bx
        jnz  dcRepetir
        inc  si
        cmp  si, 8
        jb   dcFila

        pop  es
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        pop  bp
        ret  10
DibujarCaracter ENDP

;---------------------------------------------------------------------
; DibujarTexto
;   Recibe:    SI = cadena terminada en 0 (en DS), AX = x, BX = y,
;              DL = color, DH = escala (1..3)
;   Devuelve:  cadena dibujada de izquierda a derecha
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarTexto PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        mov  cx, ax             ; CX = x del proximo caracter
        mov  al, dh
        xor  ah, ah
        mov  di, ax
        shl  di, 3              ; DI = ancho de un caracter = 8 * escala
dtSiguiente:
        lodsb
        or   al, al
        jz   dtFin
        xor  ah, ah
        push ax                 ; caracter
        push cx                 ; x
        push bx                 ; y
        mov  al, dl
        push ax                 ; color (AH sigue en 0)
        mov  al, dh
        push ax                 ; escala
        call DibujarCaracter
        add  cx, di
        jmp  dtSiguiente
dtFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarTexto ENDP

;---------------------------------------------------------------------
; CentrarTexto
;   Recibe:    SI = cadena terminada en 0, BX = y, DL = color, DH = escala
;   Devuelve:  cadena centrada horizontalmente en los 320 px
;   Destruye:  nada
;---------------------------------------------------------------------
CentrarTexto PROC
        push ax
        push cx
        push si

        xor  cx, cx             ; CX = cantidad de caracteres
ctContar:
        lodsb
        or   al, al
        jz   ctListo
        inc  cx
        jmp  ctContar
ctListo:
        mov  al, dh
        xor  ah, ah
        shl  ax, 3              ; AX = 8 * escala
        push dx                 ; MUL destruye DX, que trae color y escala
        mul  cx                 ; AX = ancho total en pixeles
        pop  dx
        mov  cx, ANCHO_PANT
        sub  cx, ax
        shr  cx, 1              ; CX = (320 - ancho) / 2
        pop  si                 ; volver al inicio de la cadena
        push si
        mov  ax, cx
        call DibujarTexto

        pop  si
        pop  cx
        pop  ax
        ret
CentrarTexto ENDP
;---------------------------------------------------------------------
; DibujarNumero
;   Recibe:    AL = numero 0..99, CX = x, BX = y, DL = color, DH = escala
;   Devuelve:  el numero dibujado (1 o 2 digitos, sin ceros a la izquierda).
;              Si tiene 1 digito agrega un espacio: con fondoTexto activo cubre
;              el segundo digito de un numero anterior mas largo (10 -> 9).
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarNumero PROC
        push ax
        push si

        xor  ah, ah
        div  diez               ; AL = decenas, AH = unidades (DL y DH no cambian)
        mov  si, OFFSET numBuf
        or   al, al
        jz   dnUnidad
        add  al, '0'
        mov  [si], al           ; decenas, solo si no son 0
        inc  si
dnUnidad:
        add  ah, '0'
        mov  [si], ah
        inc  si
        cmp  si, OFFSET numBuf + 1
        jne  dnCerrar           ; dos digitos: no falta nada
        mov  BYTE PTR [si], ' ' ; un digito: espacio final. Con fondoTexto activo
        inc  si                 ; borra el segundo digito del numero anterior
dnCerrar:
        mov  BYTE PTR [si], 0   ; fin de la cadena
        mov  si, OFFSET numBuf
        mov  ax, cx             ; DibujarTexto recibe la x en AX
        call DibujarTexto
        pop  si
        pop  ax
        ret
DibujarNumero ENDP
