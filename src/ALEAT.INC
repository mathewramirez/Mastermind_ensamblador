; ALEAT.INC - Generador pseudoaleatorio y codigo secreto (RF-04, RF-05)}
; SembrarAleatorio
;   Recibe:    nada
;   Devuelve:  semilla actualizada con el contador de tics del BIOS
;   Destruye:  nada

SembrarAleatorio PROC
        push ax
        push cx
        push dx
        mov  ah, 00h
        int  1Ah                ; CX:DX = tics
        mov  ax, dx
        xor  ax, cx             ; mezclar tambien la palabra alta
        add  semilla, ax
        pop  dx
        pop  cx
        pop  ax
        ret
SembrarAleatorio ENDP

; Aleatorio
;   Recibe:    BL = rango (1..255)
;   Devuelve:  AL = numero pseudoaleatorio en 0..BL-1
;   Destruye:  AX (AL es el resultado)

Aleatorio PROC
        push dx
        mov  ax, 25173
        mul  semilla            ; DX:AX = 25173 * semilla (solo importa AX)
        add  ax, 13849
        mov  semilla, ax        ; nuevo estado del generador
        mov  al, ah             ; quedarse con el byte alto
        xor  ah, ah             ; AX = 0..255
        div  bl                 ; AL = cociente, AH = resto
        mov  al, ah             ; AL = resto = 0..BL-1
        pop  dx
        ret
Aleatorio ENDP

; GenerarSecreto
;   Recibe:    numPos, numColores, repeticion (de ConfigurarNivel)
;   Devuelve:  secreto[0..numPos-1] con IDs de color 1..numColores
;   Destruye:  nada
;
; Si repeticion = 0 (nivel Facil) cada color sorteado se busca entre
; las posiciones ya llenas y, si ya esta, se sortea de nuevo.
GenerarSecreto PROC
        push ax
        push bx
        push cx
        push si

        call SembrarAleatorio
        xor  si, si             ; SI = posicion que se esta llenando
gsPos:
        mov  bl, numColores
        call Aleatorio          
        inc  al                 ; AL = ID de color  (el 0 es "vacio")
        cmp  repeticion, 0
        jne  gsGuardar          ; con repeticion permitida no se revisa nada
        xor  bx, bx             ; BX = indice para revisar posiciones previas
gsBuscar:
        cmp  bx, si
        jae  gsGuardar          ; revisadas todas sin encontrarlo: sirve
        cmp  al, secreto[bx]
        je   gsPos              ; repetido: sortear de nuevo esta posicion
        inc  bx
        jmp  gsBuscar
gsGuardar:
        mov  secreto[si], al
        inc  si
        mov  cl, numPos
        xor  ch, ch
        cmp  si, cx
        jb   gsPos              ; faltan posiciones

        pop  si
        pop  cx
        pop  bx
        pop  ax
        ret
GenerarSecreto ENDP