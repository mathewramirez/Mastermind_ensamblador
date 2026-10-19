;=====================================================================
; PRUEBA4.ASM - Pruebas de la Fase 4: ciclo de partida (modo texto)
; Herramienta de desarrollo: NO es parte del juego entregado.
;   Parte A: pruebas automaticas (victoria, derrota, ultimo intento,
;            intento incompleto, historial, intentos restantes).
;   Parte B: partida jugable en consola, con secreto manual opcional.
; Ensamblar:  BUILD.bat src\PRUEBA4
;=====================================================================
        .MODEL SMALL
        .286
        .STACK 200h

        INCLUDE CONST.INC
        INCLUDE MACROS.INC

        .DATA
        INCLUDE SPRITES.INC     ; DATOS.INC usa sus etiquetas (tablaFichas)
        INCLUDE DATOS.INC

fallos      DB 0                ; cuantas verificaciones fallaron

; Codigos de prueba (se copian a secreto[] / intento[])
cod1234     DB 1,2,3,4,5        ; secreto 1,2,3,4 (el 5 solo lo usa Dificil)
codParc     DB 4,3,2,1,0        ; mismos simbolos, todos en otra posicion
codCasi     DB 1,2,3,5,0        ; 3 exactos
codIncomp   DB 1,2,0,4,0        ; casilla vacia

msgTitulo   DB 'PRUEBAS FASE 4 - Ciclo de partida',13,10,'$'
msgParteA   DB 13,10,'--- Parte A: pruebas automaticas ---',13,10,'$'
msgParteB   DB 13,10,'--- Parte B: partida en consola ---',13,10,'$'
msgOk       DB ': OK',13,10,'$'
msgMal      DB ': FALLO  obtenido=$'
msgEsper    DB '  esperado=$'
msgResumen  DB 13,10,'Verificaciones fallidas: $'
msgNivel    DB 'Nivel (0=facil 1=normal 2=dificil): $'
msgManual   DB 'Escribir el secreto a mano? (S/N): $'
msgSecreto  DB 'Escribe el secreto (digitos 1..C, uno por posicion): $'
msgAyuda    DB 'Flechas/digitos arman el intento, BACKSPACE borra, ENTER confirma, ESC sale.',13,10,'$'
msgTurno    DB 'Intento $'
msgDe       DB ' de $'
msgIntento  DB '  armando:$'
msgIncomp   DB '  ** Intento incompleto: NO se gasta turno',13,10,'$'
msgRes      DB '   ->  exactos=$'
msgParc     DB '  parciales=$'
msgGano     DB 13,10,'*** VICTORIA ***',13,10,'$'
msgPerdio   DB 13,10,'*** DERROTA ***  El secreto era:$'
msgFin      DB 'Fin de la prueba',13,10,'$'

tA1 DB 'A1 exactos del 1er intento (3)$'
tA2 DB 'A2 parciales del 1er intento (0)$'
tA3 DB 'A3 turno tras 1 intento (1)$'
tA4 DB 'A4 sigue JUGANDO (0)$'
tA5 DB 'A5 historial[0][3] guardado (5)$'
tA6 DB 'A6 intento queda vacio (0)$'
tA7 DB 'A7 parciales del 2do intento (4)$'
tA8 DB 'A8 exactos del 3er intento (4)$'
tA9 DB 'A9 estado VICTORIA (1)$'
tA10 DB 'A10 turno al ganar (3)$'
tA11 DB 'A11 histExactos[2] (4)$'
tA12 DB 'A12 intentos restantes al ganar (7)$'
tB1 DB 'B1 Normal: tras 9 fallos sigue JUGANDO (0)$'
tB2 DB 'B2 Normal: 10 fallos -> DERROTA (2)$'
tB3 DB 'B3 Normal: turno final (10)$'
tC1 DB 'C1 Facil: tras 11 fallos sigue JUGANDO (0)$'
tC2 DB 'C2 Facil: 12 fallos -> DERROTA (2)$'
tD1 DB 'D1 Dificil: 9 fallos sigue JUGANDO (0)$'
tD2 DB 'D2 Dificil: acertar en el intento 10 -> VICTORIA (1)$'
tE1 DB 'E1 ENTER con intento incompleto -> RES_INCOMPLETO (2)$'
tE2 DB 'E2 el turno NO avanza (0)$'

        .CODE
inicio:
        mov  ax, @data
        mov  ds, ax
        cld

        mov  dx, OFFSET msgTitulo
        call ImprimirCadena
        mov  dx, OFFSET msgParteA
        call ImprimirCadena
        call PruebasAutomaticas
        mov  dx, OFFSET msgResumen
        call ImprimirCadena
        mov  al, fallos
        call ImprimirDecimal
        call NuevaLinea

        mov  dx, OFFSET msgParteB
        call ImprimirCadena
        call PartidaConsola

        mov  dx, OFFSET msgFin
        call ImprimirCadena
        mov  ax, 4C00h
        int  21h

;---------------------------------------------------------------------
; PruebasAutomaticas
;   Ejecuta los escenarios A..E y verifica cada resultado.
;   Recibe: nada.  Devuelve: fallos actualizado.  Destruye: nada
;---------------------------------------------------------------------
PruebasAutomaticas PROC
        push ax
        push cx
        push dx
        push si

        ;---- A: Normal, 3 intentos, el ultimo gana ----
        mov  nivel, NIVEL_NORMAL
        call IniciarPartida
        mov  si, OFFSET cod1234
        call PonerSecreto
        mov  si, OFFSET codCasi
        call PonerIntento
        call ConfirmarIntento   ; AH = exactos, DL = parciales, AL = estado
        push ax
        push dx
        mov  al, ah
        mov  ah, 3
        mov  dx, OFFSET tA1
        call Verificar
        pop  dx
        mov  al, dl
        xor  ah, ah
        mov  dx, OFFSET tA2
        call Verificar
        pop  ax
        mov  al, turno
        mov  ah, 1
        mov  dx, OFFSET tA3
        call Verificar
        mov  al, estado
        mov  ah, ESTADO_JUGANDO
        mov  dx, OFFSET tA4
        call Verificar
        mov  al, historial[3]   ; fila 0, columna 3
        mov  ah, 5
        mov  dx, OFFSET tA5
        call Verificar
        mov  al, intento[0]
        mov  ah, 0
        mov  dx, OFFSET tA6
        call Verificar

        mov  si, OFFSET codParc
        call PonerIntento
        call ConfirmarIntento
        mov  al, dl
        mov  ah, 4
        mov  dx, OFFSET tA7
        call Verificar

        mov  si, OFFSET cod1234
        call PonerIntento
        call ConfirmarIntento
        mov  al, ah
        mov  ah, 4
        mov  dx, OFFSET tA8
        call Verificar
        mov  al, estado
        mov  ah, ESTADO_VICTORIA
        mov  dx, OFFSET tA9
        call Verificar
        mov  al, turno
        mov  ah, 3
        mov  dx, OFFSET tA10
        call Verificar
        mov  al, histExactos[2]
        mov  ah, 4
        mov  dx, OFFSET tA11
        call Verificar
        call IntentosRestantes
        mov  ah, 7
        mov  dx, OFFSET tA12
        call Verificar

        ;---- B: Normal, 10 fallos -> derrota ----
        mov  nivel, NIVEL_NORMAL
        call IniciarPartida
        mov  si, OFFSET cod1234
        call PonerSecreto
        mov  cx, 9
        call FallarVeces
        mov  al, estado
        mov  ah, ESTADO_JUGANDO
        mov  dx, OFFSET tB1
        call Verificar
        mov  cx, 1
        call FallarVeces
        mov  al, estado
        mov  ah, ESTADO_DERROTA
        mov  dx, OFFSET tB2
        call Verificar
        mov  al, turno
        mov  ah, 10
        mov  dx, OFFSET tB3
        call Verificar

        ;---- C: Facil, 12 fallos -> derrota ----
        mov  nivel, NIVEL_FACIL
        call IniciarPartida
        mov  si, OFFSET cod1234
        call PonerSecreto
        mov  cx, 11
        call FallarVeces
        mov  al, estado
        mov  ah, ESTADO_JUGANDO
        mov  dx, OFFSET tC1
        call Verificar
        mov  cx, 1
        call FallarVeces
        mov  al, estado
        mov  ah, ESTADO_DERROTA
        mov  dx, OFFSET tC2
        call Verificar

        ;---- D: Dificil (N=5), gana justo en el ultimo intento ----
        mov  nivel, NIVEL_DIFICIL
        call IniciarPartida
        mov  si, OFFSET cod1234
        call PonerSecreto       ; secreto 1,2,3,4,5
        mov  cx, 9
        call FallarVeces
        mov  al, estado
        mov  ah, ESTADO_JUGANDO
        mov  dx, OFFSET tD1
        call Verificar
        mov  si, OFFSET cod1234
        call PonerIntento
        call ConfirmarIntento
        mov  al, estado
        mov  ah, ESTADO_VICTORIA
        mov  dx, OFFSET tD2
        call Verificar

        ;---- E: ENTER con intento incompleto no gasta turno ----
        mov  nivel, NIVEL_NORMAL
        call IniciarPartida
        mov  si, OFFSET codIncomp
        call PonerIntento
        mov  al, ACC_CONFIRMAR
        call ProcesarAccion
        mov  ah, RES_INCOMPLETO
        mov  dx, OFFSET tE1
        call Verificar
        mov  al, turno
        mov  ah, 0
        mov  dx, OFFSET tE2
        call Verificar

        pop  si
        pop  dx
        pop  cx
        pop  ax
        ret
PruebasAutomaticas ENDP

;---------------------------------------------------------------------
; PonerSecreto / PonerIntento
;   Recibe:    SI = offset de una tabla de bytes; numPos
;   Devuelve:  secreto[] (o intento[]) con los primeros numPos bytes
;   Destruye:  nada
;---------------------------------------------------------------------
PonerSecreto PROC
        push ax
        push bx
        push cx
        mov  cl, numPos
        xor  ch, ch
        xor  bx, bx
psLoop:
        mov  al, [si+bx]
        mov  secreto[bx], al
        inc  bx
        loop psLoop
        pop  cx
        pop  bx
        pop  ax
        ret
PonerSecreto ENDP

PonerIntento PROC
        push ax
        push bx
        push cx
        mov  cl, numPos
        xor  ch, ch
        xor  bx, bx
piLoop:
        mov  al, [si+bx]
        mov  intento[bx], al
        inc  bx
        loop piLoop
        pop  cx
        pop  bx
        pop  ax
        ret
PonerIntento ENDP

;---------------------------------------------------------------------
; FallarVeces
;   Recibe:    CX = cuantos intentos equivocados confirmar
;   Devuelve:  esos intentos registrados (todos con el simbolo 6, que
;              no esta en el secreto de prueba: 0 exactos, 0 parciales)
;   Destruye:  nada
;---------------------------------------------------------------------
FallarVeces PROC
        push ax
        push bx
        push cx
        push dx
fvOtro:
        push cx
        mov  cl, numPos
        xor  ch, ch
        xor  bx, bx
fvLlenar:
        mov  intento[bx], 6
        inc  bx
        loop fvLlenar
        call ConfirmarIntento
        pop  cx
        loop fvOtro
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
FallarVeces ENDP

;---------------------------------------------------------------------
; Verificar
;   Recibe:    AL = obtenido, AH = esperado, DX = nombre ('$')
;   Devuelve:  imprime OK o FALLO; si falla, fallos++
;   Destruye:  nada
;---------------------------------------------------------------------
Verificar PROC
        push ax
        push dx
        push ax
        call ImprimirCadena     ; nombre de la verificacion
        pop  ax
        cmp  al, ah
        jne  veMal
        mov  dx, OFFSET msgOk
        call ImprimirCadena
        jmp  veFin
veMal:
        inc  fallos
        push ax
        mov  dx, OFFSET msgMal
        call ImprimirCadena
        call ImprimirDecimal    ; obtenido (AL)
        mov  dx, OFFSET msgEsper
        call ImprimirCadena
        pop  ax
        mov  al, ah
        call ImprimirDecimal    ; esperado
        call NuevaLinea
veFin:
        pop  dx
        pop  ax
        ret
Verificar ENDP

;---------------------------------------------------------------------
; PartidaConsola
;   Partida jugable en texto: arma intentos con el teclado, confirma
;   con ENTER y muestra el resultado hasta ganar, perder o ESC.
;   Recibe: nada.  Devuelve: nada.  Destruye: nada
;---------------------------------------------------------------------
PartidaConsola PROC
        push ax
        push dx
        call ElegirNivel
        call IniciarPartida
        call PreguntarSecretoManual
        mov  dx, OFFSET msgAyuda
        call ImprimirCadena
        call VaciarTeclado
pcTurno:
        call MostrarTurno
pcTecla:
        call EsperarTecla
        call TraducirTecla
        call ProcesarAccion
        cmp  al, RES_SALIR
        je   pcSalir
        cmp  al, RES_INCOMPLETO
        je   pcIncompleto
        cmp  al, RES_CONFIRMAR
        je   pcConfirmar
        mov  dx, OFFSET msgIntento     ; cambio de casilla/simbolo: mostrar
        call ImprimirCadena
        call MostrarIntento
        call NuevaLinea
        jmp  pcTecla
pcIncompleto:
        mov  dx, OFFSET msgIncomp
        call ImprimirCadena
        jmp  pcTecla
pcConfirmar:
        call MostrarIntentoFinal       ; antes de que ConfirmarIntento lo limpie
        call ConfirmarIntento          ; AL = estado, AH = exactos, DL = parciales
        push ax
        push dx                        ; ImprimirCadena usa DX: guardar parciales
        mov  dx, OFFSET msgRes
        call ImprimirCadena
        mov  al, ah
        call ImprimirDecimal
        mov  dx, OFFSET msgParc
        call ImprimirCadena
        pop  dx
        mov  al, dl
        call ImprimirDecimal
        call NuevaLinea
        pop  ax
        cmp  al, ESTADO_VICTORIA
        je   pcGano
        cmp  al, ESTADO_DERROTA
        je   pcPerdio
        jmp  pcTurno
pcGano:
        mov  dx, OFFSET msgGano
        call ImprimirCadena
        jmp  pcSalir
pcPerdio:
        mov  dx, OFFSET msgPerdio
        call ImprimirCadena
        call MostrarSecreto
        call NuevaLinea
pcSalir:
        pop  dx
        pop  ax
        ret
PartidaConsola ENDP

;---------------------------------------------------------------------
; MostrarTurno
;   Imprime "Intento k de M" (k = turno+1).
;   Recibe: turno, maxIntentos.  Devuelve: nada.  Destruye: nada
;---------------------------------------------------------------------
MostrarTurno PROC
        push ax
        push dx
        mov  dx, OFFSET msgTurno
        call ImprimirCadena
        mov  al, turno
        inc  al
        call ImprimirDecimal
        mov  dx, OFFSET msgDe
        call ImprimirCadena
        mov  al, maxIntentos
        call ImprimirDecimal
        call NuevaLinea
        pop  dx
        pop  ax
        ret
MostrarTurno ENDP

;---------------------------------------------------------------------
; PreguntarSecretoManual
;   Permite escribir el secreto a mano (para los casos del profesor).
;   Acepta solo digitos 1..C; si responde N deja el aleatorio.
;   Recibe: numPos, numColores.  Devuelve: secreto[].  Destruye: nada
;---------------------------------------------------------------------
PreguntarSecretoManual PROC
        push ax
        push bx
        push cx
        push dx
        mov  dx, OFFSET msgManual
        call ImprimirCadena
psmTecla:
        call EsperarTecla
        or   al, 20h            ; a minuscula
        cmp  al, 'n'
        je   psmNo
        cmp  al, 's'
        jne  psmTecla
        call NuevaLinea
        mov  dx, OFFSET msgSecreto
        call ImprimirCadena
        mov  cl, numPos
        xor  ch, ch
        xor  bx, bx
psmDigito:
        call EsperarTecla
        cmp  al, '1'
        jb   psmDigito
        mov  ah, al
        sub  ah, '0'
        cmp  ah, numColores
        ja   psmDigito          ; fuera de 1..C: se ignora
        mov  secreto[bx], ah
        mov  dl, al
        call ImprimirCaracter
        inc  bx
        loop psmDigito
psmNo:
        call NuevaLinea
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
PreguntarSecretoManual ENDP

;---------------------------------------------------------------------
; MostrarIntentoFinal
;   Imprime el intento que se va a confirmar. Recibe: intento[].
;   Devuelve: nada.  Destruye: nada
;---------------------------------------------------------------------
MostrarIntentoFinal PROC
        push dx
        mov  dx, OFFSET msgIntento
        call ImprimirCadena
        call MostrarIntento
        pop  dx
        ret
MostrarIntentoFinal ENDP

;---------------------------------------------------------------------
; MostrarSecreto
;   Imprime los numPos simbolos del secreto. Devuelve: nada. Destruye: nada
;---------------------------------------------------------------------
MostrarSecreto PROC
        push bx
        push cx
        push dx
        mov  cl, numPos
        xor  ch, ch
        xor  bx, bx
msLoop:
        mov  dl, ' '
        call ImprimirCaracter
        mov  dl, secreto[bx]
        add  dl, '0'
        call ImprimirCaracter
        inc  bx
        loop msLoop
        pop  dx
        pop  cx
        pop  bx
        ret
MostrarSecreto ENDP

;---------------------------------------------------------------------
; ElegirNivel
;   Recibe: nada.  Devuelve: nivel (0..2).  Destruye: nada
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
        call ImprimirCaracter
        sub  al, '0'
        mov  nivel, al
        call NuevaLinea
        pop  dx
        pop  ax
        ret
ElegirNivel ENDP

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
        mov  dl, '_'
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
; ImprimirDecimal
;   Recibe:    AL = numero 0..99
;   Devuelve:  imprime sus digitos.  Destruye: nada
;---------------------------------------------------------------------
ImprimirDecimal PROC
        push ax
        push dx
        xor  ah, ah
        mov  dl, 10
        div  dl                 ; AL = decenas, AH = unidades
        mov  dh, ah             ; guardar unidades
        or   al, al
        jz   idUnidad           ; sin cero a la izquierda
        mov  dl, al
        add  dl, '0'
        call ImprimirCaracter
idUnidad:
        mov  dl, dh
        add  dl, '0'
        call ImprimirCaracter
        pop  dx
        pop  ax
        ret
ImprimirDecimal ENDP

ImprimirCadena PROC
        push ax
        mov  ah, 09h
        int  21h
        pop  ax
        ret
ImprimirCadena ENDP

ImprimirCaracter PROC
        push ax
        mov  ah, 02h
        int  21h
        pop  ax
        ret
ImprimirCaracter ENDP

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
        INCLUDE CICLO.INC

        END inicio