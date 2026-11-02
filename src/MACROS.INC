; MACROS.INC - Macros del proyecto (RT-04)


; DIBUJAR_SPRITE etiqueta, ancho, alto, x, y
;   Apila los 5 parametros en el orden que espera DibujarSprite y la
;   llama. DibujarSprite los saca de la pila con "ret 10".
DIBUJAR_SPRITE MACRO etiqueta, ancho, alto, px, py
        push OFFSET etiqueta
        push ancho
        push alto
        push px
        push py
        call DibujarSprite
ENDM

; LIMPIAR_ARREGLO arreglo, cuantos
;   Pone en 0 'cuantos' bytes de 'arreglo'.
;   Requiere: AL = 0, ES = DS y DF = 0 
;   Destruye: CX, DI
LIMPIAR_ARREGLO MACRO arreglo, cuantos
        mov  di, OFFSET arreglo
        mov  cx, cuantos
        rep  stosb
ENDM


; RELLENAR_RECT color, x, y, ancho, alto
;   Apila los 5 parametros y llama a RellenarRect (mismo estilo que
;   DIBUJAR_SPRITE). Los parametros pueden ser constantes o registros.

RELLENAR_RECT MACRO color, px, py, ancho, alto
        push color
        push px
        push py
        push ancho
        push alto
        call RellenarRect
ENDM

; TEXTO cadena, x, y, color, escala
;   Dibuja una cadena (terminada en 0) en (x,y).
;   Destruye: AX, BX, DX, SI
TEXTO MACRO cadena, px, py, color, escala
        mov  si, OFFSET cadena
        mov  ax, px
        mov  bx, py
        mov  dl, color
        mov  dh, escala
        call DibujarTexto
ENDM

; CENTRAR cadena, y, color, escala
;   Igual que TEXTO pero centrado horizontalmente en los 320 px.
;   Destruye: BX, DX, SI
CENTRAR MACRO cadena, py, color, escala
        mov  si, OFFSET cadena
        mov  bx, py
        mov  dl, color
        mov  dh, escala
        call CentrarTexto
ENDM