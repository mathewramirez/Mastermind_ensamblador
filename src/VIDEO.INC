;=====================================================================
; VIDEO.INC - Manejo del modo de video y dibujo basico
;=====================================================================

;---------------------------------------------------------------------
; GuardarModoVideo
;   Recibe:    nada
;   Devuelve:  modoOriginal = modo de video actual (INT 10h, AH=0Fh)
;   Destruye:  nada
;---------------------------------------------------------------------
GuardarModoVideo PROC
        push ax
        push bx                 ; la funcion 0Fh devuelve la pagina activa en BH
        mov  ah, 0Fh
        int  10h                ; AL = modo actual
        mov  modoOriginal, al
        pop  bx
        pop  ax
        ret
GuardarModoVideo ENDP

;---------------------------------------------------------------------
; EntrarModo13h
;   Recibe:    nada
;   Devuelve:  pantalla en modo 13h (320x200x256), limpia en negro
;   Destruye:  nada
;---------------------------------------------------------------------
EntrarModo13h PROC
        push ax
        mov  ax, 0013h          ; AH=00h "fijar modo", AL=13h el modo grafico
        int  10h
        pop  ax
        ret
EntrarModo13h ENDP

;---------------------------------------------------------------------
; RestaurarModoVideo
;   Recibe:    modoOriginal (guardado por GuardarModoVideo)
;   Devuelve:  la pantalla vuelve al modo con el que arranco DOS
;   Destruye:  nada
;---------------------------------------------------------------------
RestaurarModoVideo PROC
        push ax
        mov  ah, 00h
        mov  al, modoOriginal
        int  10h
        pop  ax
        ret
RestaurarModoVideo ENDP

;---------------------------------------------------------------------
; PonerPixel
;   Recibe:    AX = x (0..319), BX = y (0..199), DL = indice de color
;   Devuelve:  nada
;   Destruye:  nada
;
; offset = y*320 + x. En vez de MUL (lento y que ensucia DX, justo
; donde viene el color) usamos: y*320 = y*256 + y*64 = (y<<8)+(y<<6)
;---------------------------------------------------------------------
PonerPixel PROC
        push cx
        push di
        push es

        mov  di, bx
        shl  di, 8              ; DI = y*256
        mov  cx, bx
        shl  cx, 6              ; CX = y*64
        add  di, cx             ; DI = y*320
        add  di, ax             ; DI = y*320 + x  -> desplazamiento del pixel

        mov  cx, VIDEO_SEG
        mov  es, cx             ; ES apunta a la memoria de video (no se puede mov es, inmediato)
        mov  es:[di], dl        ; escribir el byte = encender el pixel

        pop  es
        pop  di
        pop  cx
        ret
PonerPixel ENDP

;---------------------------------------------------------------------
; LimpiarPantalla
;   Recibe:    AL = indice de color de relleno
;   Devuelve:  los 64 000 pixeles con ese color
;   Destruye:  nada
;
; Escribe de a 2 pixeles por instruccion (STOSW guarda AX en ES:DI y
; suma 2 a DI). 64 000 bytes = 32 000 palabras. Por eso AH = AL.
;---------------------------------------------------------------------
LimpiarPantalla PROC
        push ax
        push cx
        push di
        push es

        mov  ah, al             ; AX = color:color (dos pixeles iguales)
        mov  cx, VIDEO_SEG
        mov  es, cx
        xor  di, di             ; DI = 0, esquina superior izquierda
        mov  cx, (ANCHO_PANT * ALTO_PANT) / 2
        rep  stosw              ; repetir CX veces: ES:[DI] = AX, DI += 2

        pop  es
        pop  di
        pop  cx
        pop  ax
        ret
LimpiarPantalla ENDP

;---------------------------------------------------------------------
; CargarPaleta  (RG-07)
;   Recibe:    tablaPaleta (R,G,B por entrada), PALETA_INICIO, PALETA_CANT
;   Devuelve:  colores PALETA_INICIO.. redefinidos en el DAC del VGA
;   Destruye:  nada
;
; Protocolo del DAC: OUT 3C8h = primer indice a escribir; luego se
; mandan R,G,B por 3C9h y el VGA avanza SOLO al siguiente indice cada
; 3 bytes. Por eso basta un ciclo de PALETA_CANT*3 escrituras.
;---------------------------------------------------------------------
CargarPaleta PROC
        push ax
        push cx
        push dx
        push si

        mov  dx, 3C8h
        mov  al, PALETA_INICIO
        out  dx, al             ; "voy a escribir a partir de este indice"
        inc  dx                 ; DX = 3C9h, puerto de datos del DAC
        mov  si, OFFSET tablaPaleta
        mov  cx, PALETA_CANT * 3
cpSiguiente:
        lodsb                   ; AL = DS:[SI], SI++
        out  dx, al
        loop cpSiguiente

        pop  si
        pop  dx
        pop  cx
        pop  ax
        ret
CargarPaleta ENDP

;---------------------------------------------------------------------
; DibujarSprite  (RG-02, RG-03)
;   Recibe por PILA (apilar en este orden, o usar la macro DIBUJAR_SPRITE):
;       push OFFSET sprite      ; [bp+12] puntero a los bytes del sprite (en DS)
;       push ancho              ; [bp+10]
;       push alto               ; [bp+8]
;       push x                  ; [bp+6]  esquina superior izquierda
;       push y                  ; [bp+4]
;       call DibujarSprite
;   Devuelve:  nada. Saca sus 5 parametros con "ret 10" (5 palabras)
;   Destruye:  nada
;
; Pila al entrar, despues de "push bp / mov bp, sp":
;   [bp+0] BP viejo   [bp+2] direccion de retorno   [bp+4] y ... [bp+12] sprite
;
; Color 0 = transparente: ese byte NO se escribe, asi se ve el fondo.
;---------------------------------------------------------------------
DibujarSprite PROC
        push bp
        mov  bp, sp
        push ax
        push cx
        push dx
        push si
        push di
        push es

        mov  ax, VIDEO_SEG
        mov  es, ax

        ; DI = y*320 + x  (misma cuenta que PonerPixel)
        mov  di, [bp+4]
        mov  ax, di
        shl  di, 8
        shl  ax, 6
        add  di, ax
        add  di, [bp+6]

        mov  si, [bp+12]        ; SI recorre los bytes del sprite
        mov  dx, [bp+8]         ; DX = filas que faltan
dsFila:
        mov  cx, [bp+10]        ; CX = columnas de esta fila
dsColumna:
        lodsb                   ; AL = siguiente pixel del sprite
        or   al, al             ; pone ZF=1 si AL==0, sin cambiar AL
        jz   dsTransparente
        mov  es:[di], al
dsTransparente:
        inc  di
        loop dsColumna

        add  di, ANCHO_PANT     ; bajar una fila en pantalla...
        sub  di, [bp+10]        ; ...y regresar a la x inicial
        dec  dx
        jnz  dsFila

        pop  es
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  ax
        pop  bp
        ret  10                 ; retorna y descarta los 10 bytes de parametros
DibujarSprite ENDP
