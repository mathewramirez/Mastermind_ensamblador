;=====================================================================
; MACROS.INC - Macros del proyecto (RT-04)
; Una macro NO es un procedimiento: el ensamblador copia su cuerpo en
; cada lugar donde se usa (no hay CALL/RET). Sirve para ahorrar
; escritura repetida, como apilar los parametros antes de un CALL.
;=====================================================================

;---------------------------------------------------------------------
; DIBUJAR_SPRITE etiqueta, ancho, alto, x, y
;   Apila los 5 parametros en el orden que espera DibujarSprite y la
;   llama. DibujarSprite los saca de la pila con "ret 10".
;   Ej:  DIBUJAR_SPRITE CAFE_0, CAFE_ANCHO, CAFE_ALTO, 20, 92
;---------------------------------------------------------------------
DIBUJAR_SPRITE MACRO etiqueta, ancho, alto, px, py
        push OFFSET etiqueta
        push ancho
        push alto
        push px
        push py
        call DibujarSprite
ENDM
