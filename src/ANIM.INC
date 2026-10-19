
;=====================================================================
; ANIM.INC - Animacion de 2 cuadros controlada por el reloj del BIOS
; (RG-05, RG-06). La velocidad depende de los tics (18.2 por segundo),
; no de la velocidad de la CPU: en DOSBox y en un PC real es igual.
; Sirve para cualquier sprite de 32x32: la mascota (presentacion y
; panel), la victoria y la derrota. Se elige con animSpr0/animSpr1.
;=====================================================================

;---------------------------------------------------------------------
; ObtenerTics
;   Recibe:    nada
;   Devuelve:  AX = parte baja del contador de tics (INT 1Ah, AH=00h)
;   Destruye:  AX
;---------------------------------------------------------------------
ObtenerTics PROC
        push cx
        push dx
        mov  ah, 00h
        int  1Ah                ; CX:DX = tics desde la medianoche
        mov  ax, dx
        pop  dx
        pop  cx
        ret
ObtenerTics ENDP

;---------------------------------------------------------------------
; DibujarMascota
;   Recibe:    cuadroAnim (0/1), animSpr0/animSpr1 (sprites de 32x32),
;              animX, animY, animFondo (color con el que se borra)
;   Devuelve:  el cuadro actual en (animX, animY); borra antes SOLO su
;              rectangulo de 32x32 (sin parpadeo, RG-08)
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarMascota PROC
        push ax
        push bx
        push cx
        push dx
        push si

        mov  al, animFondo
        xor  ah, ah
        RELLENAR_RECT ax, animX, animY, MASCOTA_ANCHO, MASCOTA_ALTO
        mov  si, animSpr0
        cmp  cuadroAnim, 0
        je   dmDibujar
        mov  si, animSpr1
dmDibujar:
        push si                 ; puntero al sprite del cuadro actual
        push MASCOTA_ANCHO
        push MASCOTA_ALTO
        push animX
        push animY
        call DibujarSprite

        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarMascota ENDP

;---------------------------------------------------------------------
; ActualizarMascota
;   Recibe:    ticsUlt, cuadroAnim
;   Devuelve:  si pasaron TICS_ANIM tics desde el ultimo cambio, alterna
;              el cuadro y lo dibuja; si no, no hace nada
;   Destruye:  nada
;---------------------------------------------------------------------
ActualizarMascota PROC
        push ax
        push bx
        call ObtenerTics
        mov  bx, ax
        sub  bx, ticsUlt        ; diferencia sin signo (tolera el desborde)
        cmp  bx, TICS_ANIM
        jb   amFin
        mov  ticsUlt, ax
        xor  cuadroAnim, 1
        call DibujarMascota
amFin:
        pop  bx
        pop  ax
        ret
ActualizarMascota ENDP

;---------------------------------------------------------------------
; IniciarAnimacion
;   Recibe:    SI = sprite del cuadro 0, DI = sprite del cuadro 1,
;              AX = x, BX = y, DL = color de fondo
;   Devuelve:  animacion configurada, cuadro 0 dibujado y reloj en marcha
;   Destruye:  nada
;---------------------------------------------------------------------
IniciarAnimacion PROC
        push ax
        mov  animSpr0, si
        mov  animSpr1, di
        mov  animX, ax
        mov  animY, bx
        mov  animFondo, dl
        mov  cuadroAnim, 0
        call DibujarMascota
        call ObtenerTics
        mov  ticsUlt, ax
        pop  ax
        ret
IniciarAnimacion ENDP

;---------------------------------------------------------------------
; EsperarTeclaAnimada
;   Recibe:    la animacion ya iniciada con IniciarAnimacion
;   Devuelve:  AH = scan code, AL = ASCII de la tecla pulsada
;   Destruye:  AX
;   Mientras no hay tecla (INT 16h AH=01h no bloquea) sigue animando.
;---------------------------------------------------------------------
EsperarTeclaAnimada PROC
etaEspera:
        call ActualizarMascota
        call HayTecla
        jz   etaEspera
        call EsperarTecla
        ret
EsperarTeclaAnimada ENDP