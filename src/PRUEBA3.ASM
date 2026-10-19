;=====================================================================
; PRUEBA3.ASM - Pruebas de la Fase 3: teclado (modo texto)
; Herramienta de desarrollo: NO es parte del juego entregado.
;   Muestra, por cada tecla: lo que devuelve INT 16h (AH=scan, AL=ASCII),
;   la accion que reconoce TraducirTecla y como queda el intento.
;   La casilla activa se muestra entre [ ]; una vacia se muestra con _.
; Ensamblar:  BUILD.bat src\PRUEBA3
;=====================================================================
        .MODEL SMALL
        .286
        .STACK 200h

        INCLUDE CONST.INC
        INCLUDE MACROS.INC

        .DATA
        INCLUDE SPRITES.INC     ; DATOS.INC usa sus etiquetas (tablaFichas)
        INCLUDE DATOS.INC

hexa        DB '0123456789ABCDEF'

; Nombre de cada accion, indexado por ACC_xxx * 2 (tabla de punteros)
nombresAcc  DW OFFSET nNinguna, OFFSET nIzq,    OFFSET nDer
            DW OFFSET nArriba,  OFFSET nAbajo,  OFFSET nSimbolo
            DW OFFSET nBorrar,  OFFSET nConfirm, OFFSET nSalir
nNinguna    DB 'ninguna (ignorada)$'
nIzq        DB 'IZQUIERDA         $'
nDer        DB 'DERECHA           $'
nArriba     DB 'SIMBOLO SIGUIENTE $'
nAbajo      DB 'SIMBOLO ANTERIOR  $'
nSimbolo    DB 'PONER SIMBOLO     $'
nBorrar     DB 'BORRAR            $'
nConfirm    DB 'CONFIRMAR         $'
nSalir      DB 'SALIR             $'

msgTitulo   DB 'PRUEBAS FASE 3 - Teclado',13,10,'$'
msgNivel    DB 'Nivel (0=facil 1=normal 2=dificil): $'
msgConfig   DB 'Posiciones=$'
msgColores  DB '  Simbolos validos: 1..$'
msgAyuda    DB 'Flechas <- -> mueven, flechas arriba/abajo cambian el simbolo,',13,10
            DB 'digitos ponen simbolo, BACKSPACE borra, ENTER confirma, ESC sale.',13,10,'$'
msgInicio   DB 'Estado inicial                          intento:$'
msgAH       DB 'AH=$'
msgAL       DB ' AL=$'
msgFlecha   DB ' -> $'
msgIntento  DB ' intento:$'
msgIncomp   DB '   ** INCOMPLETO: hay casillas vacias, NO se gasta turno',13,10,'$'
msgComp     DB '   ** COMPLETO: listo para evaluar (se hara en la Fase 4)',13,10,'$'
msgFin      DB 'ESC: fin de la prueba',13,10,'$'

        .CODE
inicio:
        mov  ax, @data
        mov  ds, ax
        cld

        mov  dx, OFFSET msgTitulo
        call ImprimirCadena
        call ElegirNivel
        call IniciarPartida     ; configura N y C del nivel y limpia el intento
        call MostrarConfig
        call VaciarTeclado
        mov  dx, OFFSET msgInicio
        call ImprimirCadena
        call MostrarIntento
        call NuevaLinea
        call CicloTeclas

        mov  ax, 4C00h
        int  21h

;---------------------------------------------------------------------
; CicloTeclas
;   Lee teclas hasta ESC; por cada una muestra codigos, accion e intento.
;   Recibe: nada.  Devuelve: nada.  Destruye: nada
;---------------------------------------------------------------------
CicloTeclas PROC
        push ax
        push bx
        push dx
ctLeer:
        call EsperarTecla       ; AH = scan code, AL = ASCII
        call MostrarCodigos     ; imprime AH y AL tal como llegaron
        call TraducirTecla      ; AL = accion, AH = simbolo (si aplica)

        mov  bl, al
        xor  bh, bh
        shl  bx, 1              ; cada puntero de la tabla mide 2 bytes
        mov  dx, nombresAcc[bx]
        call ImprimirCadena     ; nombre de la accion reconocida

        call ProcesarAccion     ; AL = resultado RES_xxx
        mov  dx, OFFSET msgIntento
        call ImprimirCadena
        call MostrarIntento
        call NuevaLinea

        cmp  al, RES_INCOMPLETO
        jne  ctNoIncomp
        mov  dx, OFFSET msgIncomp
        call ImprimirCadena
        jmp  ctLeer
ctNoIncomp:
        cmp  al, RES_CONFIRMAR
        jne  ctNoComp
        mov  dx, OFFSET msgComp
        call ImprimirCadena
        jmp  ctLeer
ctNoComp:
        cmp  al, RES_SALIR
        jne  ctLeer
        mov  dx, OFFSET msgFin
        call ImprimirCadena
        pop  dx
        pop  bx
        pop  ax
        ret
CicloTeclas ENDP

;---------------------------------------------------------------------
; ElegirNivel
;   Pide 0, 1 o 2; cualquier otra tecla se ignora.
;   Recibe: nada.  Devuelve: nivel.  Destruye: nada
;---------------------------------------------------------------------
ElegirNivel PROC
        push ax
        push dx
        mov  dx, OFFSET msgNivel
        call ImprimirCadena
enLeer:
        call EsperarTecla
        cmp  al, '0'
        jb   enLeer
        cmp  al, '2'
        ja   enLeer
        mov  dl, al
        call ImprimirCaracter   ; eco del nivel elegido
        sub  al, '0'
        mov  nivel, al
        call NuevaLinea
        pop  dx
        pop  ax
        ret
ElegirNivel ENDP

;---------------------------------------------------------------------
; MostrarConfig
;   Imprime N y el rango de simbolos validos del nivel.
;   Recibe: numPos, numColores.  Devuelve: nada.  Destruye: nada
;---------------------------------------------------------------------
MostrarConfig PROC
        push dx
        mov  dx, OFFSET msgConfig
        call ImprimirCadena
        mov  dl, numPos
        add  dl, '0'
        call ImprimirCaracter
        mov  dx, OFFSET msgColores
        call ImprimirCadena
        mov  dl, numColores
        add  dl, '0'
        call ImprimirCaracter
        call NuevaLinea
        mov  dx, OFFSET msgAyuda
        call ImprimirCadena
        pop  dx
        ret
MostrarConfig ENDP

;---------------------------------------------------------------------
; MostrarCodigos
;   Recibe:    AX = tecla (AH = scan code, AL = ASCII)
;   Devuelve:  nada (imprime "AH=xx AL=xx -> ")
;   Destruye:  nada
;---------------------------------------------------------------------
MostrarCodigos PROC
        push ax
        push dx
        mov  dx, OFFSET msgAH
        call ImprimirCadena
        push ax
        mov  al, ah
        call ImprimirHex
        pop  ax
        mov  dx, OFFSET msgAL
        call ImprimirCadena
        call ImprimirHex
        mov  dx, OFFSET msgFlecha
        call ImprimirCadena
        pop  dx
        pop  ax
        ret
MostrarCodigos ENDP

;---------------------------------------------------------------------
; MostrarIntento
;   Imprime las numPos casillas; la activa entre [ ], vacia como _.
;   Recibe: intento[], posActiva, numPos.  Devuelve: nada.  Destruye: nada
;---------------------------------------------------------------------
MostrarIntento PROC
        push ax
        push bx
        push cx
        push dx
        mov  cl, numPos
        xor  ch, ch
        xor  bx, bx
miCasilla:
        mov  dl, ' '
        cmp  bl, posActiva
        jne  miAbre
        mov  dl, '['
miAbre:
        call ImprimirCaracter
        mov  dl, intento[bx]
        or   dl, dl
        jnz  miNumero
        mov  dl, '_'            ; casilla vacia
        jmp  miValor
miNumero:
        add  dl, '0'
miValor:
        call ImprimirCaracter
        mov  dl, ' '
        cmp  bl, posActiva
        jne  miCierra
        mov  dl, ']'
miCierra:
        call ImprimirCaracter
        inc  bx
        loop miCasilla
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
MostrarIntento ENDP

;---------------------------------------------------------------------
; ImprimirHex
;   Recibe:    AL = byte
;   Devuelve:  nada (imprime 2 digitos hexadecimales)
;   Destruye:  nada
;---------------------------------------------------------------------
ImprimirHex PROC
        push ax
        push bx
        push dx
        mov  ah, al             ; guardar el byte completo
        mov  bl, al
        shr  bl, 4              ; nibble alto
        xor  bh, bh
        mov  dl, hexa[bx]
        call ImprimirCaracter
        mov  bl, ah
        and  bl, 0Fh            ; nibble bajo
        mov  dl, hexa[bx]
        call ImprimirCaracter
        pop  dx
        pop  bx
        pop  ax
        ret
ImprimirHex ENDP

;---------------------------------------------------------------------
; ImprimirCadena
;   Recibe:    DX = offset de una cadena terminada en '$'
;   Devuelve:  nada.  Destruye: nada
;---------------------------------------------------------------------
ImprimirCadena PROC
        push ax
        mov  ah, 09h
        int  21h
        pop  ax
        ret
ImprimirCadena ENDP

;---------------------------------------------------------------------
; ImprimirCaracter
;   Recibe:    DL = caracter
;   Devuelve:  nada.  Destruye: nada
;---------------------------------------------------------------------
ImprimirCaracter PROC
        push ax
        mov  ah, 02h
        int  21h
        pop  ax
        ret
ImprimirCaracter ENDP

;---------------------------------------------------------------------
; NuevaLinea
;   Recibe: nada.  Devuelve: nada.  Destruye: nada
;---------------------------------------------------------------------
NuevaLinea PROC
        push dx
        mov  dl, 13
        call ImprimirCaracter
        mov  dl, 10
        call ImprimirCaracter
        pop  dx
        ret
NuevaLinea ENDP

        INCLUDE TECLADO.INC
        INCLUDE ALEAT.INC
        INCLUDE JUEGO.INC
        INCLUDE ENTRADA.INC

        END inicio