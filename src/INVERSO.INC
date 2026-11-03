; INVERSO.INC - Modo inverso (opcional): el jugador piensa un codigo y el
; programa lo deduce aplicando una estrategia de DESCARTE.
;
; Como funciona:
;  1) El jugador escribe su codigo al empezar. El programa NO lo usa para
;     deducir: solo lo usa para comprobar que las respuestas del jugador
;     sean correctas. Luego el programa propone un primer intento fijo
;     (invApertura) y el jugador le responde cuantos toros (exactos) y
;     vacas (parciales) tiene.
;  2) Antes de aceptar la respuesta, se calculan los toros y vacas reales
;     del intento contra el codigo del jugador. Si no coinciden, se avisa
;     y no se pinta ni se gasta ningun turno: el jugador corrige.
;  3) Cada respuesta correcta se guarda en el historial. A partir de ahi
;     el programa recorre TODOS los codigos posibles en orden (como un
;     cuentakilometros en base C) y descarta los que no habrian dado
;     exactamente esas respuestas. El primero que sobrevive es el
;     siguiente intento.
;  4) Un codigo descartado nunca vuelve a servir (el historial solo
;     crece), por eso la busqueda continua desde donde quedo: en total
;     se recorren los codigos una sola vez durante toda la partida.
;
; Comparar un candidato con un intento anterior NO usa EvaluarIntento:
; toros = posiciones iguales; vacas = suma de min(veces que aparece cada
; simbolo en el candidato, en el intento) menos los toros. Los conteos de
; cada intento se calculan una vez (histCnt) y los del candidato solo
; cuando pasa la prueba de toros: es lo que lo hace lo bastante rapido
; para el nivel Dificil (8^5 = 32768 codigos) en un 8086.

; InvIniciar
;   Recibe:    nivel
;   Devuelve:  configuracion del nivel, arreglos en 0, turno = 0,
;              estado = jugando, intento[] = primer intento de la
;              apertura e invCand[] = 1,1,...,1 (primer codigo por probar)
;   Destruye:  nada
InvIniciar PROC
        push ax
        push bx
        push cx
        push si
        push di
        push es

        call ConfigurarNivel
        push ds
        pop  es
        cld
        xor  al, al
        LIMPIAR_ARREGLO intento,       MAX_N
        LIMPIAR_ARREGLO historial,     MAX_INTENTOS*MAX_N
        LIMPIAR_ARREGLO histExactos,   MAX_INTENTOS
        LIMPIAR_ARREGLO histParciales, MAX_INTENTOS
        LIMPIAR_ARREGLO histCnt,       MAX_INTENTOS*9
        mov  turno, 0
        mov  estado, ESTADO_JUGANDO
        mov  posActiva, 0FFh    ; ninguna casilla activa: no hay cursor
        mov  di, OFFSET invCand
        mov  cx, MAX_N
        mov  al, 1
        rep  stosb              ; primer candidato = 1,1,...,1

        mov  al, nivel
        mov  bl, MAX_N
        mul  bl
        mov  bx, ax             ; BX = inicio de la apertura de este nivel
        xor  si, si
iiAper:
        mov  al, invApertura[bx+si]
        mov  intento[si], al
        inc  si
        cmp  si, MAX_N
        jb   iiAper

        pop  es
        pop  di
        pop  si
        pop  cx
        pop  bx
        pop  ax
        ret
InvIniciar ENDP

; InvSiguiente
;   Recibe:    invCand[], numPos, numColores
;   Devuelve:  invCand[] = el siguiente codigo (la ultima posicion avanza
;              mas rapido, con acarreo, simbolos 1..C); AL = 1 si hay
;              siguiente, 0 si se acabaron los codigos
;   Destruye:  AX
InvSiguiente PROC
        push si
        mov  al, numPos
        xor  ah, ah
        mov  si, ax
        dec  si                 ; SI = ultima posicion
isSube:
        inc  invCand[si]
        mov  al, invCand[si]
        cmp  al, numColores
        jbe  isHay
        mov  invCand[si], 1     ; se paso de C: vuelve a 1 y acarrea a la izquierda
        dec  si
        jns  isSube
        xor  al, al             ; el acarreo salio por la izquierda: no hay mas codigos
        pop  si
        ret
isHay:
        mov  al, 1
        pop  si
        ret
InvSiguiente ENDP

; InvContarGuess
;   Recibe:    AL = fila del historial (intento ya guardado)
;   Devuelve:  histCnt[fila*9 + s] = cuantas veces aparece el simbolo s
;              (1..8) en ese intento
;   Destruye:  nada
InvContarGuess PROC
        push ax
        push bx
        push cx
        push si
        push di

        xor  ah, ah
        mov  bx, ax
        shl  bx, 3
        add  bx, ax             ; BX = fila * 9
        mov  cl, MAX_N
        mul  cl                 ; AX = fila * MAX_N = inicio de la fila en historial
        mov  di, ax
        mov  cx, 9
        xor  si, si
icgLimpia:
        mov  histCnt[bx+si], 0
        inc  si
        loop icgLimpia
        mov  cl, numPos
        xor  ch, ch
icgCuenta:
        mov  al, historial[di]
        xor  ah, ah
        mov  si, ax
        inc  histCnt[bx+si]
        inc  di
        loop icgCuenta

        pop  di
        pop  si
        pop  cx
        pop  bx
        pop  ax
        ret
InvContarGuess ENDP

; InvConsistente
;   Recibe:    invCand[] (candidato), historial[], histExactos[],
;              histParciales[], histCnt[], turno
;   Devuelve:  AL = 1 si el candidato habria dado EXACTAMENTE las
;              respuestas del jugador a todos los intentos anteriores
;              (y, en Facil, no repite simbolos); AL = 0 si se descarta
;   Destruye:  AX
;   Registros: CX = N, DL = intento que se compara, DH = 1 cuando los
;   conteos del candidato (invCnt) ya estan calculados.
InvConsistente PROC
        push bx
        push cx
        push dx
        push si
        push di

        mov  cl, numPos
        xor  ch, ch             ; CX = N
        cmp  repeticion, 0
        jne  icHist
        ; Facil: un codigo con simbolos repetidos no es posible
        xor  si, si
icI:
        mov  di, si
        inc  di
        cmp  di, cx
        jae  icISig
        mov  al, invCand[si]
icJ:
        cmp  al, invCand[di]
        jne  icJSig
        jmp  icRech
icJSig:
        inc  di
        cmp  di, cx
        jb   icJ
icISig:
        inc  si
        cmp  si, cx
        jb   icI

icHist:
        xor  dl, dl             ; DL = intento anterior que se compara
        xor  dh, dh             ; conteos del candidato aun sin calcular
icT:
        cmp  dl, turno
        jb   icComparar
        jmp  icOk               ; ya coincidio con todos
icComparar:
        ; toros: posiciones iguales
        mov  al, dl
        mov  ah, MAX_N
        mul  ah
        mov  bx, ax             ; BX = inicio de esa fila del historial
        xor  si, si
        mov  invTmp, 0
icTor:
        mov  al, invCand[si]
        cmp  al, historial[bx+si]
        jne  icNoT
        inc  invTmp
icNoT:
        inc  si
        cmp  si, cx
        jb   icTor
        mov  bl, dl
        xor  bh, bh
        mov  al, invTmp
        cmp  al, histExactos[bx]
        je   icToroOk
        jmp  icRech             ; no da los toros que dijo el jugador: descartado
icToroOk:
        ; conteos del candidato (una sola vez por candidato)
        or   dh, dh
        jnz  icSuma
        xor  si, si
icLimp:
        mov  invCnt[si], 0
        inc  si
        cmp  si, 9
        jb   icLimp
        xor  si, si
icCnt:
        mov  al, invCand[si]
        xor  ah, ah
        mov  di, ax
        inc  invCnt[di]
        inc  si
        cmp  si, cx
        jb   icCnt
        mov  dh, 1
icSuma:
        ; vacas = suma de los minimos de cada simbolo, menos los toros
        mov  al, dl
        xor  ah, ah
        mov  bx, ax
        shl  bx, 3
        add  bx, ax             ; BX = intento * 9
        push cx
        mov  cl, numColores
        xor  ch, ch             ; CX = C mientras se recorren los simbolos
        xor  ah, ah             ; AH = suma acumulada
        mov  si, 1
icCol:
        mov  al, invCnt[si]
        cmp  al, histCnt[bx+si]
        jbe  icMin
        mov  al, histCnt[bx+si]
icMin:
        add  ah, al
        inc  si
        cmp  si, cx
        jbe  icCol
        pop  cx                 ; CX = N otra vez
        sub  ah, invTmp         ; AH = vacas que habria dado
        mov  bl, dl
        xor  bh, bh
        cmp  ah, histParciales[bx]
        je   icParcialOk
        jmp  icRech
icParcialOk:
        inc  dl
        jmp  icT
icOk:
        mov  al, 1
        jmp  icvFin
icRech:
        xor  al, al
icvFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
InvConsistente ENDP

; InvBuscar
;   Recibe:    invCand[] (por donde va la busqueda), historial[]
;   Devuelve:  AL = 1 y intento[] = el siguiente codigo que sobrevive al
;              descarte; AL = 0 si no queda ninguno y entonces intento[]
;              no se modifica
;   Destruye:  AX
; Como las respuestas se validan contra el codigo del jugador, el codigo
; real siempre sobrevive: AL = 0 no deberia ocurrir.
InvBuscar PROC
        push cx
        push si
ibProbar:
        call InvConsistente
        or   al, al
        jnz  ibHallado
        call InvSiguiente
        or   al, al
        jnz  ibProbar
        pop  si                 ; AL = 0: se acabaron los codigos
        pop  cx
        ret
ibHallado:
        mov  cl, numPos
        xor  ch, ch
        xor  si, si
ibCopia:
        mov  al, invCand[si]
        mov  intento[si], al    ; el candidato pasa a ser el intento del programa
        inc  si
        loop ibCopia
        mov  al, 1
        pop  si
        pop  cx
        ret
InvBuscar ENDP

; InvRespuestaCorrecta
;   Recibe:    intento[] (lo que propuso el programa), secreto[] (el
;              codigo del jugador), invResp[] (toros y vacas que dijo)
;   Devuelve:  AL = 1 si lo que dijo el jugador coincide con los toros y
;              vacas reales; AL = 0 si se equivoco
;   Destruye:  AX
; Es el unico uso que el modo inverso le da a secreto[]: no interviene en
; la deduccion del programa, solo en detectar el error del jugador.
InvRespuestaCorrecta PROC
        call EvaluarIntento     ; AL = toros reales, AH = vacas reales
        cmp  al, invResp
        jne  ircNo
        cmp  ah, invResp + 1
        jne  ircNo
        mov  al, 1
        ret
ircNo:
        xor  al, al
        ret
InvRespuestaCorrecta ENDP

; DibujarCampoInv
;   Recibe:    AL = campo (0 toros, 1 vacas), invCampo, invResp[]
;   Devuelve:  el recuadro de ese campo (dorado si es el activo) y su
;              numero. Se pinta sin borrar antes (RG-08): el numero lleva
;              fondo de panel y el recuadro solo cambia de color.
;   Destruye:  nada
; La respuesta nunca pasa de N (maximo 5), asi que es un solo digito. Se
; dibuja a la derecha, lejos de la etiqueta, sin usar DibujarNumero porque
; este agrega un espacio final que taparia el borde del recuadro.
DibujarCampoInv PROC
        push ax
        push bx
        push cx
        push dx
        push si

        xor  ah, ah
        mov  si, ax             ; SI = campo
        mov  dl, C_PANEL        ; recuadro apagado
        cmp  al, invCampo
        jne  dciMarco
        mov  dl, C_DORADO       ; recuadro del campo activo
dciMarco:
        mov  cl, 19
        mul  cl
        add  ax, 34
        mov  bx, ax             ; BX = y del recuadro (18 px de alto)
        RELLENAR_RECT dx, 208, bx, 104, 1
        RELLENAR_RECT dx, 208, bx, 1, 18
        RELLENAR_RECT dx, 311, bx, 1, 18
        add  bx, 17
        RELLENAR_RECT dx, 208, bx, 104, 1
        sub  bx, 16             ; y del numero: dentro del recuadro, bajo el borde

        mov  ax, C_PANEL
        mov  fondoTexto, ax
        mov  al, invResp[si]
        add  al, '0'            ; digito 0..5 como caracter
        mov  numBuf, al
        mov  numBuf + 1, 0      ; cadena de un solo caracter
        mov  si, OFFSET numBuf
        mov  ax, 287            ; x del digito (la etiqueta termina en 279)
        mov  dl, C_TEXTO
        mov  dh, 2
        call DibujarTexto
        mov  fondoTexto, 0FFFFh

        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarCampoInv ENDP

; DibujarEntradaInv
;   Recibe:    invCampo, invResp[]
;   Devuelve:  los dos campos (toros y vacas) redibujados
;   Destruye:  nada
DibujarEntradaInv PROC
        push ax
        xor  al, al
        call DibujarCampoInv
        mov  al, 1
        call DibujarCampoInv
        pop  ax
        ret
DibujarEntradaInv ENDP

; DibujarCodigoInv
;   Recibe:    numPos, secreto[] (el codigo que penso el jugador, completo)
;   Devuelve:  las fichas del codigo en el panel, centradas, en y = 82.
;              Cada ficha queda a 20 px de la anterior.
;   Destruye:  nada
DibujarCodigoInv PROC
        push ax
        push bx
        push cx
        push dx
        push si

        mov  al, numPos
        dec  al
        mov  bl, 20
        mul  bl                 ; AX = (N-1) * 20
        add  ax, FICHA_LADO     ; AX = ancho total de la fila de fichas
        mov  cx, PAN_W
        sub  cx, ax
        shr  cx, 1
        add  cx, PAN_X          ; CX = x inicial, centrada en el panel
        xor  si, si
cdLazo:
        mov  ax, si
        mov  bl, 20
        mul  bl
        add  ax, cx
        mov  dx, ax             ; DX = x de esta ficha
        mov  al, secreto[si]
        push cx
        mov  cx, dx
        mov  bx, 82
        call DibujarFichaSobre
        pop  cx
        inc  si
        mov  ax, si
        cmp  al, numPos
        jb   cdLazo

        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarCodigoInv ENDP

; DibujarPantallaInv
;   Recibe:    nivel, partida ya iniciada con InvIniciar, y el codigo del
;              jugador en secreto[] (PedirCodigoInv)
;   Devuelve:  encabezado, tablero con el historial, panel con los campos
;              de respuesta, el codigo del jugador, el intento del programa
;              y los contadores
;   Destruye:  nada
DibujarPantallaInv PROC
        push ax
        push bx
        push dx
        push si
        push di

        mov  si, OFFSET tVacio
        mov  di, OFFSET tInvAyuda
        call PintarPantalla
        TEXTO tHdrInv, 8, 6, C_DORADO, 1
        TEXTO tNivelH, 200, 6, C_TEXTO, 1
        mov  bl, nivel
        xor  bh, bh
        shl  bx, 1
        mov  si, nombresNivel[bx]
        mov  ax, 256
        mov  bx, 6
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarTexto

        RELLENAR_RECT C_FILA,  TAB_X, TAB_Y, TAB_W, TAB_H
        RELLENAR_RECT C_FONDO, TAB_X+1, TAB_Y+1, TAB_W-2, TAB_H-2
        RELLENAR_RECT C_BORDE, PAN_X, PAN_Y, PAN_W, PAN_H
        RELLENAR_RECT C_PANEL, PAN_X+1, PAN_Y+1, PAN_W-2, PAN_H-2
        TEXTO tInvTu, 212, 24, C_DORADO, 1
        DIBUJAR_SPRITE EXACTO_0, EXACTO_ANCHO, EXACTO_ALTO, 214, 39
        TEXTO tToros, 224, 39, C_TEXTO, 1
        DIBUJAR_SPRITE PARCIAL_0, PARCIAL_ANCHO, PARCIAL_ALTO, 214, 58
        TEXTO tVacas, 224, 58, C_TEXTO, 1
        TEXTO tInvCod, 220, 73, C_DORADO, 1
        TEXTO tInvMi, 220, 99, C_DORADO, 1

        call DibujarTablero
        call DibujarEntradaInv
        call DibujarCodigoInv
        call DibujarIntentoActual
        call DibujarContadores
        mov  si, OFFSET MASCOTA_0
        mov  di, OFFSET MASCOTA_1
        mov  ax, 244
        mov  bx, 156
        mov  dl, C_PANEL
        call IniciarAnimacion

        pop  di
        pop  si
        pop  dx
        pop  bx
        pop  ax
        ret
DibujarPantallaInv ENDP

; DibujarTituloInv
;   Recibe:    estado, cuadroAnim
;   Devuelve:  "LO ADIVINE" (dorado/blanco con la animacion) si el
;              programa acerto, o "ME GANASTE" (rojo) si no, a escala 3
;   Destruye:  nada
DibujarTituloInv PROC
        push bx
        push dx
        push si
        cmp  estado, ESTADO_VICTORIA
        jne  dtiPerdio
        mov  dl, C_DORADO
        cmp  cuadroAnim, 0
        je   dtiAcerto
        mov  dl, C_TEXTO
dtiAcerto:
        mov  si, OFFSET tInvGano
        jmp  dtiDibujar
dtiPerdio:
        mov  dl, C_ROJO
        mov  si, OFFSET tInvPerdio
dtiDibujar:
        mov  bx, 80
        mov  dh, 3
        call CentrarTexto
        pop  si
        pop  dx
        pop  bx
        ret
DibujarTituloInv ENDP

; PantallaFinInv
;   Recibe:    estado (VICTORIA = el programa acerto, DERROTA = no acerto
;              en sus intentos), turno
;   Devuelve:  AL = 1 jugar otra vez (ENTER), 0 volver al menu (ESC o M)
;   Destruye:  AX
PantallaFinInv PROC
        push bx
        push cx
        push dx
        push si
        push di

        mov  si, OFFSET tHdrFinInv
        mov  di, OFFSET tBarraFin
        call PintarPantalla
        mov  si, OFFSET VICTORIA_0
        mov  di, OFFSET VICTORIA_1
        cmp  estado, ESTADO_VICTORIA
        je   pfiAnim
        mov  si, OFFSET DERROTA_0
        mov  di, OFFSET DERROTA_1
pfiAnim:
        mov  ax, 144
        mov  bx, 30
        mov  dl, C_FONDO
        call IniciarAnimacion
        cmp  estado, ESTADO_VICTORIA
        jne  pfiNoAcerto
        TEXTO tUsados, 84, 124, C_TEXTO, 1
        mov  al, turno
        mov  cx, 220
        mov  bx, 124
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarNumero
        cmp  puntPuesto, 0FFh
        je   pfiSinRecord       ; solo se anuncia si entro al top 5 de la computadora
        TEXTO tRecordFin, 72, 138, C_DORADO, 1
        mov  al, puntPuesto
        inc  al                 ; puesto desde 1
        mov  cx, 240
        mov  bx, 138
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarNumero
pfiSinRecord:
        mov  si, OFFSET sndVictoria
        jmp  pfiTitulo
pfiNoAcerto:
        CENTRAR tInvFalla, 124, C_TEXTO, 1
        mov  si, OFFSET sndDerrota
pfiTitulo:
        call DibujarTituloInv
        call VaciarTeclado
        call IniciarSonido      ; SI = melodia elegida arriba
pfiEspera:
        call ActualizarSonido
        mov  al, cuadroAnim
        call ActualizarMascota
        cmp  al, cuadroAnim
        je   pfiTecla
        call DibujarTituloInv   ; el titulo cambia de color con el cuadro
pfiTecla:
        call HayTecla
        jz   pfiEspera
        call EsperarTecla
        cmp  al, ASC_ENTER
        je   pfiOtra
        cmp  al, ASC_ESC
        je   pfiMenu
        or   al, 20h            ; a minuscula
        cmp  al, 'm'
        jne  pfiEspera
pfiMenu:
        xor  al, al
        jmp  pfiFin
pfiOtra:
        mov  al, 1
pfiFin:
        call DetenerSonido
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
PantallaFinInv ENDP

; InvAvisar
;   Recibe:    SI = mensaje (<= 40 caracteres), DL = color
;   Devuelve:  el mensaje en la barra de estado; msgVis = 1
;   Destruye:  nada
InvAvisar PROC
        mov  msgVis, 1
        call MostrarEstado
        ret
InvAvisar ENDP

; InvAceptar
;   Recibe:    invResp[] (toros, vacas), intento[] (lo que propuso el
;              programa), turno
;   Devuelve:  la respuesta guardada en el historial y dibujada en el
;              tablero, turno avanzado y estado actualizado
;              (VICTORIA si dijo N toros; DERROTA si se agotaron los
;              intentos)
;   Destruye:  AX
InvAceptar PROC
        mov  al, invResp
        mov  ah, invResp + 1
        call GuardarIntento     ; intento[] + toros/vacas en la fila 'turno'
        mov  al, turno
        call InvContarGuess
        inc  turno
        mov  al, turno
        dec  al
        call DibujarFilaHistorial
        call DibujarContadores
        mov  al, invResp
        cmp  al, numPos
        jne  iaNoGano
        mov  estado, ESTADO_VICTORIA
        ret
iaNoGano:
        mov  al, turno
        cmp  al, maxIntentos
        jb   iaSigue
        mov  estado, ESTADO_DERROTA
iaSigue:
        ret
InvAceptar ENDP

; InvCambiarCampo
;   Recibe:    AL = ACC_ARRIBA o ACC_ABAJO, invCampo, invResp[], numPos
;   Devuelve:  el valor del campo activo +1 o -1 (da la vuelta entre
;              0 y N); redibuja los campos
;   Destruye:  AX
InvCambiarCampo PROC
        push bx
        mov  bl, invCampo
        xor  bh, bh
        mov  ah, invResp[bx]
        cmp  al, ACC_ARRIBA
        jne  iccBaja
        inc  ah
        cmp  ah, numPos
        jbe  iccGuardar
        xor  ah, ah             ; paso de N: vuelve a 0
        jmp  iccGuardar
iccBaja:
        or   ah, ah
        jnz  iccResta
        mov  ah, numPos
        inc  ah                 ; de 0 baja a N
iccResta:
        dec  ah
iccGuardar:
        mov  invResp[bx], ah
        pop  bx
        ret
InvCambiarCampo ENDP

; DibujarSlotsCodigo
;   Recibe:    numPos, secreto[] (0 = casilla vacia)
;   Devuelve:  las N casillas de 20x20 centradas en y = 100, cada una con
;              su ficha si ya tiene simbolo (pantalla de pedir el codigo)
;   Destruye:  nada
DibujarSlotsCodigo PROC
        push ax
        push bx
        push cx
        push dx
        push si

        mov  al, numPos
        mov  bl, 24
        mul  bl                 ; AX = ancho de la fila (24 px por casilla)
        mov  cx, 320
        sub  cx, ax
        shr  cx, 1              ; CX = x inicial centrado
        xor  si, si
scLazo:
        mov  ax, si
        mov  bl, 24
        mul  bl
        add  ax, cx
        mov  dx, ax             ; DX = x de la casilla
        RELLENAR_RECT C_FILA, dx, 100, 20, 20
        mov  al, secreto[si]
        or   al, al
        jz   scSigue            ; casilla vacia: solo el recuadro
        push cx
        mov  cx, dx
        add  cx, 2              ; ficha de 16 centrada en la casilla de 20
        mov  bx, 102
        call DibujarFichaSobre
        pop  cx
scSigue:
        inc  si
        mov  ax, si
        cmp  al, numPos
        jb   scLazo

        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarSlotsCodigo ENDP

; AvisoCodigoInv
;   Recibe:    DI = cadena (<= 40 caracteres), DL = color
;   Devuelve:  el texto en la barra de estado
;   Destruye:  nada
; Existe para no perder SI, que PedirCodigoInv usa como contador.
AvisoCodigoInv PROC
        push si
        mov  si, di
        call MostrarEstado
        pop  si
        ret
AvisoCodigoInv ENDP

; PedirCodigoInv
;   Recibe:    numPos, numColores, repeticion (ya cargados por InvIniciar)
;   Devuelve:  secreto[0..numPos-1] con el codigo que piensa el jugador;
;              AL = 1 si lo confirmo con ENTER, AL = 0 si cancelo con ESC
;   Destruye:  AX
; El programa NO usa este codigo para deducir: se muestra en el panel como
; recordatorio y sirve para comprobar las respuestas del jugador
; (InvRespuestaCorrecta). En Facil no se aceptan simbolos repetidos. El
; modo inverso no usa secreto[] para nada mas, por eso se aprovecha aqui.
PedirCodigoInv PROC
        push bx
        push cx
        push dx
        push si
        push di

        mov  si, OFFSET tHdrInv
        mov  di, OFFSET tInvBarraCod
        call PintarPantalla
        CENTRAR tInvCod1, 56, C_TEXTO, 1
        CENTRAR tInvCod2, 68, C_TEXTO, 1
        CENTRAR tInvCod3, 80, C_DORADO, 1

        xor  bx, bx             ; descartar el codigo de una partida anterior
pcLimpia:
        mov  secreto[bx], 0
        inc  bx
        cmp  bx, MAX_N
        jb   pcLimpia

        xor  si, si             ; SI = simbolos ya escritos
        call DibujarSlotsCodigo
        call VaciarTeclado
pcLeer:
        call EsperarTecla
        call TraducirTecla
        cmp  al, ACC_SALIR
        jne  pcNoEsc
        xor  al, al             ; ESC: cancelar
        jmp  pcFin
pcNoEsc:
        cmp  al, ACC_BORRAR
        je   pcBorrar
        cmp  al, ACC_CONFIRMAR
        je   pcEnter
        cmp  al, ACC_SIMBOLO
        jne  pcLeer             ; otra tecla: ignorar
        mov  dl, ah             ; DL = ID elegido (AX se destruye abajo)
        mov  ax, si
        cmp  al, numPos
        jae  pcLeer             ; ya estan todas las casillas llenas
        mov  al, dl
        call SimboloRepetido    ; en Facil no se aceptan repetidos
        or   al, al
        jnz  pcRepetido
        mov  secreto[si], dl
        inc  si
        jmp  pcRefresca
pcBorrar:
        or   si, si
        jz   pcLeer             ; no hay nada que borrar
        dec  si
        mov  secreto[si], 0
pcRefresca:
        call DibujarSlotsCodigo
        mov  di, OFFSET tInvBarraCod
        mov  ax, si
        cmp  al, numPos
        jb   pcMensaje
        mov  di, OFFSET tInvListo   ; codigo completo: falta confirmar
pcMensaje:
        mov  dl, C_TEXTO
        call AvisoCodigoInv
        jmp  pcLeer
pcRepetido:
        mov  di, OFFSET tRepetido
        mov  dl, C_DORADO
        call AvisoCodigoInv
        jmp  pcLeer
pcEnter:
        mov  ax, si
        cmp  al, numPos
        jae  pcListo
        mov  di, OFFSET tInvFalta
        mov  dl, C_DORADO
        call AvisoCodigoInv
        jmp  pcLeer
pcListo:
        mov  al, 1
pcFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
PedirCodigoInv ENDP

; PartidaInversa
;   Recibe:    nivel
;   Devuelve:  AL = 1 jugar otra vez, 0 volver al menu (ESC)
;   Destruye:  AX
;   Ciclo: pedir el codigo del jugador -> mostrar el intento del programa
;   -> leer la respuesta del jugador (flechas o digitos) -> ENTER: se
;   compara con los toros y vacas reales (si no coincide, se avisa y no se
;   pinta nada) -> se guarda la respuesta, el programa descarta y busca
;   el siguiente intento.
PartidaInversa PROC
        push bx
        push cx
        push dx
        push si
        push di

        call InvIniciar
        call CargarGeometria
        call PedirCodigoInv     ; el jugador escribe el codigo que va a pensar
        or   al, al
        jnz  pvCodigoOk
        jmp  pvAbandono         ; ESC: salto largo, la etiqueta queda lejos
pvCodigoOk:
        call DibujarPantallaInv
        call IniciarCrono       ; el tiempo cuenta desde que aparece el tablero (no se dibuja)
pvIntento:
        mov  invResp, 0         ; cada intento empieza con 0 toros y 0 vacas
        mov  invResp + 1, 0
        mov  invCampo, 0
        mov  msgVis, 0
        call DibujarIntentoActual
        call DibujarEntradaInv
        mov  si, OFFSET tInvAyuda
        mov  dl, C_TEXTO
        call MostrarEstado
        call VaciarTeclado
pvLeer:
        call ActualizarMascota
        call ActualizarSonido
        call HayTecla
        jz   pvLeer
        call EsperarTecla
        cmp  al, '0'
        jb   pvAccion
        cmp  al, '9'
        ja   pvAccion
        sub  al, '0'            ; digito escrito: se pone directo en el campo activo
        cmp  al, numPos
        ja   pvLeer             ; mas de N no tiene sentido: se ignora
        mov  bl, invCampo
        xor  bh, bh
        mov  invResp[bx], al
        mov  invCampo, 1        ; tras los toros se pasa a las vacas
        jmp  pvCambio
pvAccion:
        call TraducirTecla
        cmp  al, ACC_SALIR
        jne  pvNoEsc
        call ConfirmarSalida    ; ESC: pregunta antes de abandonar
        or   al, al
        jz   pvLeer             ; no: se sigue en la partida
        jmp  pvAbandono         ; si: salto largo, la etiqueta queda lejos
pvNoEsc:
        cmp  al, ACC_CONFIRMAR
        je   pvConfirmar
        cmp  al, ACC_IZQ
        je   pvCampo
        cmp  al, ACC_DER
        je   pvCampo
        cmp  al, ACC_ARRIBA
        je   pvValor
        cmp  al, ACC_ABAJO
        jne  pvLeer             ; cualquier otra tecla: ignorar
pvValor:
        call InvCambiarCampo
        jmp  pvCambio
pvCampo:
        xor  invCampo, 1        ; izquierda o derecha: cambia entre exactos y parciales
pvCambio:
        mov  si, OFFSET sndCambio
        call IniciarSonido
        call DibujarEntradaInv
        cmp  msgVis, 0
        je   pvLeer
        mov  msgVis, 0          ; quitar el aviso y volver a la ayuda
        mov  si, OFFSET tInvAyuda
        mov  dl, C_TEXTO
        call MostrarEstado
        jmp  pvLeer

pvConfirmar:
        call InvRespuestaCorrecta   ; compara con los toros y vacas reales
        or   al, al
        jnz  pvAcepta
        mov  si, OFFSET sndError    ; el error es del jugador: no se guarda ni
        call IniciarSonido          ; se pinta nada, y puede corregir
        mov  si, OFFSET tInvMal
        mov  dl, C_DORADO
        call InvAvisar
        jmp  pvLeer
pvAcepta:
        call InvAceptar
        cmp  estado, ESTADO_JUGANDO
        jne  pvTermino
        mov  si, OFFSET tInvPensando
        mov  dl, C_DORADO
        call MostrarEstado      ; la busqueda puede tardar unos segundos (Dificil)
        call InvBuscar
        or   al, al
        jnz  pvHayCodigo
        jmp  pvAbandono         ; no deberia pasar: el codigo del jugador siempre sobrevive
pvHayCodigo:
        mov  si, OFFSET sndConfirmar
        call IniciarSonido
        jmp  pvIntento
pvTermino:
        call DetenerSonido
        call ObtenerSegundos    ; tiempo total de la partida (no se dibuja en este modo)
        mov  tiempoFinal, ax
        mov  puntPuesto, 0FFh
        cmp  estado, ESTADO_VICTORIA
        jne  pvFinal
        call RegistrarCPU       ; si la computadora acerto, puede entrar a sus mejores puntajes
pvFinal:
        call PantallaFinInv     ; AL = 1 otra vez, 0 menu
        jmp  pvFin
pvAbandono:
        call DetenerSonido
        xor  al, al
pvFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
PartidaInversa ENDP