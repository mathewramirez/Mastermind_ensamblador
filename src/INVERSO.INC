;=====================================================================
; INVERSO.INC - Modo inverso (opcional): el jugador piensa un codigo y el
; programa lo deduce aplicando una estrategia de DESCARTE.
;
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
        ;  posiciones iguales
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
;              descarte; AL = 0 si no queda ninguno (respuestas
;              contradictorias)
;   Destruye:  AX
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

; DibujarCampoInv
;   Recibe:    AL = campo (0 toros, 1 vacas), invCampo, invResp[]
;   Devuelve:  el recuadro de ese campo (dorado si es el activo) y su
;              numero. Se pinta sin borrar antes (RG-08): el numero lleva
;              fondo de panel y el recuadro solo cambia de color.
;   Destruye:  nada
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
        mov  cl, 28
        mul  cl
        add  ax, 38
        mov  bx, ax             ; BX = y del recuadro (24 px de alto)
        RELLENAR_RECT dx, 208, bx, 104, 1
        RELLENAR_RECT dx, 208, bx, 1, 24
        RELLENAR_RECT dx, 311, bx, 1, 24
        add  bx, 23
        RELLENAR_RECT dx, 208, bx, 104, 1
        sub  bx, 19             ; y del numero: 4 px bajo el borde superior

        mov  ax, C_PANEL
        mov  fondoTexto, ax
        mov  al, invResp[si]
        mov  cx, 272
        mov  dl, C_TEXTO
        mov  dh, 2
        call DibujarNumero
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

; DibujarPantallaInv
;   Recibe:    nivel, partida ya iniciada con InvIniciar
;   Devuelve:  encabezado, tablero con el historial, panel con los campos
;              de respuesta, el intento del programa y los contadores
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
        TEXTO tInvTu, 212, 25, C_DORADO, 1
        DIBUJAR_SPRITE EXACTO_0, EXACTO_ANCHO, EXACTO_ALTO, 214, 46
        TEXTO tToros, 226, 46, C_TEXTO, 1
        DIBUJAR_SPRITE PARCIAL_0, PARCIAL_ANCHO, PARCIAL_ALTO, 214, 74
        TEXTO tVacas, 226, 74, C_TEXTO, 1
        TEXTO tInvMi, 220, 96, C_DORADO, 1

        call DibujarTablero
        call DibujarEntradaInv
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

;   Recibe:    invResp[] (toros, vacas), numPos
;   Devuelve:  AL = 1 si la respuesta puede existir; 0 si es imposible
;              (toros + vacas > N, o N-1 toros con 1 vaca)
;   Destruye:  AX
InvRespuestaValida PROC
        mov  al, invResp        ; AL = toros
        mov  ah, al
        add  ah, invResp + 1    ; AH = toros + vacas
        cmp  ah, numPos
        ja   irvNo
        mov  ah, numPos
        dec  ah                 ; AH = N-1
        cmp  al, ah
        jne  irvSi
        cmp  invResp + 1, 1
        je   irvNo              ; N-1 toros y 1 vaca no existe
irvSi:
        mov  al, 1
        ret
irvNo:
        xor  al, al
        ret
InvRespuestaValida ENDP

; PartidaInversa
;   Recibe:    nivel
;   Devuelve:  AL = 1 jugar otra vez, 0 volver al menu (ESC, o respuestas
;              contradictorias)
;   Destruye:  AX
;   Ciclo: mostrar el intento del programa -> leer la respuesta del
;   jugador (flechas o digitos) -> ENTER la valida y la guarda -> el
;   programa descarta y busca el siguiente intento.
PartidaInversa PROC
        push bx
        push cx
        push dx
        push si
        push di

        call InvIniciar
        call CargarGeometria
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
        xor  invCampo, 1        ; izquierda o derecha: cambia entre }ficha sbien pouestas y parciales
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
        call InvRespuestaValida
        or   al, al
        jnz  pvAcepta
        mov  si, OFFSET sndError
        call IniciarSonido
        mov  si, OFFSET tInvImposible
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
        mov  si, OFFSET sndError
        call IniciarSonido
        mov  si, OFFSET tInvSinSol
        mov  dl, C_DORADO
        call MostrarEstado
        mov  cl, numPos         ; sin codigo posible: vaciar "MI INTENTO"
        xor  ch, ch
        xor  si, si
pvBorra:
        mov  intento[si], 0
        inc  si
        loop pvBorra
        call DibujarIntentoActual
        call VaciarTeclado
pvSinSol:
        call ActualizarSonido
        call HayTecla
        jz   pvSinSol
        call EsperarTecla
        jmp  pvAbandono
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
