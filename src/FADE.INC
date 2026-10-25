
;=====================================================================
; FADE.INC - Transiciones de pantalla por fundido de paleta (opcional)
; Se oscurece la paleta propia (indices PALETA_INICIO..) en 16 pasos,
; reescribiendo el DAC del VGA con OUT a 3C8h/3C9h. Cada paso espera el
; retrazo vertical (puerto 3DAh, bit 3), asi la velocidad no depende de
; la CPU: 16 pasos = unos 0.23 s a 70 cuadros por segundo.
;
; Uso: PintarPantalla hace el fundido a negro y deja pendiente el fundido
; de entrada. La pantalla nueva se dibuja con la paleta apagada (no se
; ve nada) y el fundido de entrada ocurre en la primera lectura de tecla
; (HayTecla / EsperarTecla), cuando la pantalla ya esta completa.
;=====================================================================

;---------------------------------------------------------------------
; EsperarRetrazo
;   Recibe:    nada
;   Devuelve:  al empezar un retrazo vertical (puerto 3DAh, bit 3 en 1)
;   Destruye:  nada
;---------------------------------------------------------------------
EsperarRetrazo PROC
        push ax
        push dx
        mov  dx, 3DAh
erFin:
        in   al, dx
        test al, 8
        jnz  erFin              ; si ya hay un retrazo, esperar a que termine
erInicio:
        in   al, dx
        test al, 8
        jz   erInicio           ; esperar a que empiece el siguiente
        pop  dx
        pop  ax
        ret
EsperarRetrazo ENDP

;---------------------------------------------------------------------
; AplicarBrillo
;   Recibe:    BL = brillo 0..16 (16 = colores originales, 0 = negro)
;   Devuelve:  los PALETA_CANT colores propios escritos en el DAC, cada
;              componente multiplicado por BL/16
;   Destruye:  nada
;---------------------------------------------------------------------
AplicarBrillo PROC
        push ax
        push cx
        push dx
        push si

        mov  dx, 3C8h
        mov  al, PALETA_INICIO
        out  dx, al             ; primer indice a escribir
        inc  dx                 ; DX = 3C9h
        mov  si, OFFSET tablaPaleta
        mov  cx, PALETA_CANT * 3
abComp:
        lodsb                   ; AL = componente 0..63
        mul  bl                 ; AX = componente * brillo (maximo 63*16)
        shr  ax, 4              ; / 16
        out  dx, al
        loop abComp

        pop  si
        pop  dx
        pop  cx
        pop  ax
        ret
AplicarBrillo ENDP

;---------------------------------------------------------------------
; FundidoSalida
;   Recibe:    brillo (nivel actual)
;   Devuelve:  pantalla en negro; brillo = 0
;   Destruye:  nada
;---------------------------------------------------------------------
FundidoSalida PROC
        push bx
        mov  bl, brillo
fsPaso:
        or   bl, bl
        jz   fsFin              ; ya esta en negro
        dec  bl
        call EsperarRetrazo
        call AplicarBrillo
        jmp  fsPaso
fsFin:
        mov  brillo, bl
        pop  bx
        ret
FundidoSalida ENDP

;---------------------------------------------------------------------
; FundidoEntrada
;   Recibe:    brillo (nivel actual)
;   Devuelve:  paleta completa; brillo = 16
;   Destruye:  nada
;---------------------------------------------------------------------
FundidoEntrada PROC
        push bx
        mov  bl, brillo
fePaso:
        cmp  bl, 16
        jae  feFin              ; ya esta a brillo completo
        inc  bl
        call EsperarRetrazo
        call AplicarBrillo
        jmp  fePaso
feFin:
        mov  brillo, bl
        pop  bx
        ret
FundidoEntrada ENDP

;---------------------------------------------------------------------
; AplicarFundidoPendiente
;   Recibe:    fundidoPend (1 = hay un fundido de entrada por hacer)
;   Devuelve:  si estaba pendiente, la pantalla aparece con un fundido
;   Destruye:  nada
;   La llaman HayTecla y EsperarTecla: la pantalla ya esta dibujada.
;---------------------------------------------------------------------
AplicarFundidoPendiente PROC
        cmp  fundidoPend, 0
        je   afFin
        mov  fundidoPend, 0
        call FundidoEntrada
afFin:
        ret
AplicarFundidoPendiente ENDP