
;=====================================================================
; PARTIDA.INC - Partida grafica completa (Fase 6) y pantalla final
; Une el teclado (ENTRADA/TECLADO), la logica (CICLO/JUEGO) y el dibujo
; (TABLERO/PANEL). Tras cada tecla redibuja SOLO lo que cambio (RG-08).
;=====================================================================

;---------------------------------------------------------------------
; MostrarEstado
;   Recibe:    SI = cadena (<= 40 caracteres), DL = color
;   Devuelve:  barra de estado (0,190,320x10) con el texto centrado
;   Destruye:  nada
;---------------------------------------------------------------------
MostrarEstado PROC
        push ax
        push bx
        push dx
        RELLENAR_RECT C_PANEL, EST_X, EST_Y, EST_W, EST_H
        mov  bx, 191
        mov  dh, 1
        call CentrarTexto
        pop  dx
        pop  bx
        pop  ax
        ret
MostrarEstado ENDP

;---------------------------------------------------------------------
; DibujarPantallaJuego
;   Recibe:    nivel, estado de la partida ya iniciada
;   Devuelve:  encabezado, barra de ayuda, marcos del tablero y del
;              panel y todo su contenido; arranca la mascota del panel
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarPantallaJuego PROC
        push ax
        push bx
        push dx
        push si
        push di

        mov  si, OFFSET tVacio
        mov  di, OFFSET tAyudaJ
        call PintarPantalla
        TEXTO tTitulo, 8, 6, C_DORADO, 1
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
        TEXTO tPaletaL, 236, 25, C_DORADO, 1
        TEXTO tIntentoL, 228, 96, C_DORADO, 1

        call DibujarTablero
        call DibujarPaleta
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
DibujarPantallaJuego ENDP

;---------------------------------------------------------------------
; ActualizarAnimJuego
;   Recibe:    reloj del BIOS (ticsUlt) y cuadroAnim
;   Devuelve:  alterna mascota y cursor cada TICS_ANIM tics (RG-05/06)
;   Destruye:  nada
;---------------------------------------------------------------------
ActualizarAnimJuego PROC
        push ax
        mov  al, cuadroAnim
        call ActualizarMascota
        cmp  al, cuadroAnim
        je   aaFin              ; no cambio el cuadro: nada que redibujar
        call DibujarIntentoActual
aaFin:
        pop  ax
        ret
ActualizarAnimJuego ENDP

;---------------------------------------------------------------------
; PedirSecretoManual  (modo prueba, RF-12)
;   Recibe:    numPos, numColores
;   Devuelve:  secreto[] escrito con las teclas 1..C; ESC deja el
;              secreto aleatorio. Las casillas se muestran ocultas.
;   Destruye:  nada
;---------------------------------------------------------------------
PedirSecretoManual PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        mov  si, OFFSET tHdrPrueba
        mov  di, OFFSET tBarraPrueba
        call PintarPantalla
        CENTRAR tPrueba1, 60, C_TEXTO, 1
        CENTRAR tPrueba2, 72, C_TEXTO, 1
        mov  al, numPos
        mov  bl, 24
        mul  bl                 ; ancho de la fila de casillas (24 px c/u)
        mov  cx, 320
        sub  cx, ax
        shr  cx, 1              ; CX = x inicial centrado
        xor  si, si
        call VaciarTeclado
psLeer:
        mov  ax, si
        mov  bl, 24
        mul  bl
        add  ax, cx
        RELLENAR_RECT C_FILA, ax, 100, 20, 20
        call EsperarTecla
        call TraducirTecla
        cmp  al, ACC_SALIR
        je   psAleatorio
        cmp  al, ACC_SIMBOLO
        jne  psLeer             ; otra tecla: ignorar
        mov  secreto[si], ah
        mov  ax, si
        mov  bl, 24
        mul  bl
        add  ax, cx
        RELLENAR_RECT C_ACENTO, ax, 100, 20, 20   ; casilla llena, simbolo oculto
        inc  si
        mov  ax, si
        cmp  al, numPos
        jb   psLeer
        jmp  pscFin
psAleatorio:
        call GenerarSecreto
pscFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
PedirSecretoManual ENDP

;---------------------------------------------------------------------
; JugarPartida
;   Recibe:    partida iniciada con IniciarPartida
;   Devuelve:  AL = 0 abandono (ESC), 1 victoria, 2 derrota
;   Destruye:  AX
;   Ciclo: animar -> leer tecla -> ProcesarAccion -> dibujar solo el
;   cambio. ENTER con casillas vacias avisa y NO consume turno (RF-07).
;---------------------------------------------------------------------
JugarPartida PROC
        push bx
        push cx
        push dx
        push si

        call CargarGeometria
        cmp  modoPrueba, 0
        je   jpPantalla
        call PedirSecretoManual
jpPantalla:
        mov  msgVis, 0
        call DibujarPantallaJuego
        call VaciarTeclado
jpLeer:
        call ActualizarAnimJuego
        call HayTecla
        jz   jpLeer
        call EsperarTecla
        call TraducirTecla
        call ProcesarAccion
        cmp  al, RES_CAMBIO
        je   jpCambio
        cmp  al, RES_INCOMPLETO
        je   jpIncompleto
        cmp  al, RES_CONFIRMAR
        je   jpConfirmar
        cmp  al, RES_SALIR
        jne  jpLeer             ; RES_NADA: tecla invalida
        xor  al, al             ; abandono
        jmp  jpFin

jpCambio:
        call DibujarPaleta
        call DibujarIntentoActual
        cmp  msgVis, 0
        je   jpLeer
        mov  msgVis, 0          ; quitar el aviso y volver a la ayuda
        mov  si, OFFSET tAyudaJ
        mov  dl, C_TEXTO
        call MostrarEstado
        jmp  jpLeer

jpIncompleto:
        mov  msgVis, 1
        mov  si, OFFSET tIncomp
        mov  dl, C_DORADO
        call MostrarEstado
        jmp  jpLeer

jpConfirmar:
        call ConfirmarIntento   ; AL = estado
        push ax
        mov  al, turno
        dec  al                 ; fila recien evaluada
        call DibujarFilaHistorial
        call DibujarContadores
        call DibujarIntentoActual
        mov  msgVis, 0
        mov  si, OFFSET tAyudaJ
        mov  dl, C_TEXTO
        call MostrarEstado
        pop  ax
        cmp  al, ESTADO_JUGANDO
        je   jpLeer
        ; la partida termino: dejar ver el tablero antes de la pantalla final
        mov  si, OFFSET tFinJ
        mov  dl, C_DORADO
        call MostrarEstado
        call VaciarTeclado
        call EsperarTecla
        mov  al, estado         ; 1 victoria / 2 derrota
jpFin:
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
JugarPartida ENDP

;---------------------------------------------------------------------
; DibujarModoPrueba
;   Recibe:    modoPrueba
;   Devuelve:  "[MODO PRUEBA]" bajo el menu si esta activo
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarModoPrueba PROC
        push bx
        push dx
        push si
        cmp  modoPrueba, 0
        je   dmpFin
        CENTRAR tModoPrueba, 160, C_ROJO, 1
dmpFin:
        pop  si
        pop  dx
        pop  bx
        ret
DibujarModoPrueba ENDP

;---------------------------------------------------------------------
; DibujarSecretoFin
;   Recibe:    secreto[], numPos
;   Devuelve:  las fichas del secreto centradas en y = 150
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarSecretoFin PROC
        push ax
        push bx
        push cx
        push dx
        push si

        mov  al, numPos
        mov  bl, 20
        mul  bl
        mov  cx, 320
        sub  cx, ax
        shr  cx, 1              ; CX = x inicial centrado
        xor  si, si
dsLoop:
        mov  ax, si
        mov  bl, 20
        mul  bl
        add  ax, cx
        mov  dx, ax             ; DX = x de la ficha
        RELLENAR_RECT C_CELDA, dx, 150, FICHA_LADO, FICHA_LADO
        mov  al, secreto[si]
        push cx
        mov  cx, dx
        mov  bx, 150
        call DibujarFicha
        pop  cx
        inc  si
        mov  al, numPos
        xor  ah, ah
        cmp  si, ax
        jb   dsLoop

        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarSecretoFin ENDP

;---------------------------------------------------------------------
; DibujarTituloFin
;   Recibe:    estado (victoria/derrota), cuadroAnim
;   Devuelve:  "VICTORIA" (alterna dorado/blanco con la animacion) o
;              "DERROTA" (rojo) centrado en y = 80, escala 3
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarTituloFin PROC
        push bx
        push dx
        push si
        RELLENAR_RECT C_FONDO, 0, 78, 320, 28
        cmp  estado, ESTADO_VICTORIA
        jne  tfDerrota
        mov  dl, C_DORADO
        cmp  cuadroAnim, 0
        je   tfVictoria
        mov  dl, C_TEXTO
tfVictoria:
        mov  si, OFFSET tVictoria
        jmp  tfDibujar
tfDerrota:
        mov  dl, C_ROJO
        mov  si, OFFSET tDerrota
tfDibujar:
        mov  bx, 80
        mov  dh, 3
        call CentrarTexto
        pop  si
        pop  dx
        pop  bx
        ret
DibujarTituloFin ENDP

;---------------------------------------------------------------------
; PantallaFin  (RF-11, RF-12)
;   Recibe:    AL = ESTADO_VICTORIA o ESTADO_DERROTA
;   Devuelve:  AL = 1 jugar otra vez (ENTER), 0 volver al menu (ESC/M)
;   Destruye:  AX
;   Victoria: sprite feliz y titulo que alterna dorado/blanco.
;   Derrota: sprite triste, titulo rojo y el secreto revelado.
;---------------------------------------------------------------------
PantallaFin PROC
        push bx
        push cx
        push dx
        push si
        push di

        mov  cl, al             ; CL = estado (se conserva todo el ciclo)
        mov  si, OFFSET tHdrFin
        mov  di, OFFSET tBarraFin
        call PintarPantalla

        cmp  cl, ESTADO_VICTORIA
        jne  pfDerrota
        mov  si, OFFSET VICTORIA_0
        mov  di, OFFSET VICTORIA_1
        jmp  pfAnim
pfDerrota:
        mov  si, OFFSET DERROTA_0
        mov  di, OFFSET DERROTA_1
pfAnim:
        mov  ax, 144
        mov  bx, 30
        mov  dl, C_FONDO
        call IniciarAnimacion

        cmp  cl, ESTADO_VICTORIA
        jne  pfTextoDerrota
        TEXTO tUsados, 84, 124, C_TEXTO, 1   ; 16 letras + espacio + numero = 152 px, centrado
        mov  al, turno
        mov  cx, 220
        mov  bx, 124
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarNumero
        mov  cl, ESTADO_VICTORIA
        jmp  pfLeer
pfTextoDerrota:
        CENTRAR tSecreto, 130, C_TEXTO, 1
        call DibujarSecretoFin
pfLeer:
        call VaciarTeclado
        call DibujarTituloFin
pfEspera:
        mov  al, cuadroAnim
        call ActualizarMascota
        cmp  al, cuadroAnim
        je   pfTecla
        call DibujarTituloFin           ; el titulo cambia de color con el cuadro
pfTecla:
        call HayTecla
        jz   pfEspera
        call EsperarTecla
        cmp  al, ASC_ENTER
        je   pfOtra
        cmp  al, 1Bh            ; ESC
        je   pfMenu
        or   al, 20h            ; a minuscula
        cmp  al, 'm'
        jne  pfEspera
pfMenu:
        xor  al, al
        jmp  pfFin
pfOtra:
        mov  al, 1
pfFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
PantallaFin ENDP