
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
        xor  ax, ax
        call DibujarCrono       ; "Tiempo 00:00" en el encabezado

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
;   Devuelve:  alterna mascota y cursor cada TICS_ANIM tics (RG-05/06);
;              del intento solo redibuja la casilla activa (RG-08)
;   Destruye:  nada
;---------------------------------------------------------------------
ActualizarAnimJuego PROC
        push ax
        mov  al, cuadroAnim
        call ActualizarMascota
        cmp  al, cuadroAnim
        je   aaFin              ; no cambio el cuadro: nada que redibujar
        mov  al, posActiva
        call DibujarCasilla     ; solo la casilla del cursor, no las demas
aaFin:
        pop  ax
        ret
ActualizarAnimJuego ENDP

;---------------------------------------------------------------------
; SimboloRepetido
;   Recibe:    AL = ID del simbolo, SI = cuantas posiciones del secreto
;              ya estan escritas
;   Devuelve:  AL = 1 si el nivel NO permite repetidos (repeticion = 0) y
;              el simbolo ya esta en secreto[0..SI-1]; si no, AL = 0
;   Destruye:  AX
;---------------------------------------------------------------------
SimboloRepetido PROC
        push bx
        cmp  repeticion, 0
        jne  srNo               ; el nivel permite repetir: siempre sirve
        xor  bx, bx
srBuscar:
        cmp  bx, si
        jae  srNo               ; revisadas todas sin encontrarlo
        cmp  al, secreto[bx]
        je   srSi
        inc  bx
        jmp  srBuscar
srSi:
        mov  al, 1
        jmp  srFin
srNo:
        xor  al, al
srFin:
        pop  bx
        ret
SimboloRepetido ENDP

;---------------------------------------------------------------------
; PedirSecretoManual  (modo prueba, RF-12)
;   Recibe:    numPos, numColores, repeticion
;   Devuelve:  secreto[] escrito con las teclas 1..C; ESC deja el
;              secreto aleatorio. Las casillas se muestran ocultas.
;              En Facil rechaza simbolos repetidos con un aviso.
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
        mov  dl, ah             ; DL = ID elegido (AX se destruye abajo)
        mov  al, dl
        call SimboloRepetido    ; en Facil no se aceptan repetidos (RF-05)
        or   al, al
        jnz  psRepetido
        mov  secreto[si], dl
        mov  ax, si
        mov  bl, 24
        mul  bl
        add  ax, cx
        RELLENAR_RECT C_ACENTO, ax, 100, 20, 20   ; casilla llena, simbolo oculto
        push si
        mov  si, OFFSET tBarraPrueba
        mov  dl, C_TEXTO
        call MostrarEstado      ; quita el aviso si lo habia
        pop  si
        inc  si
        mov  ax, si
        cmp  al, numPos
        jb   psLeer
        jmp  pscFin
psRepetido:
        push si
        mov  si, OFFSET tRepetido
        mov  dl, C_DORADO
        call MostrarEstado      ; aviso en la barra de estado
        pop  si
        jmp  psLeer             ; la posicion sigue vacia
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
        call IniciarCrono       ; el tiempo cuenta desde que aparece el tablero
        call VaciarTeclado
jpLeer:
        call ActualizarAnimJuego
        call ActualizarCrono
        call ActualizarSonido   ; pasa a la siguiente nota si vencio la actual
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
        mov  si, OFFSET sndCambio
        call IniciarSonido
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
        mov  si, OFFSET sndError
        call IniciarSonido
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
        jne  jpTermino
        mov  si, OFFSET sndConfirmar
        call IniciarSonido
        jmp  jpLeer
jpTermino:
        ; la partida termino: congelar el cronometro y dejar ver el tablero
        call FinalizarCrono
        mov  si, OFFSET tFinJ
        mov  dl, C_DORADO
        call MostrarEstado
        call DetenerSonido      ; EsperarTecla bloquea: no dejar una nota sonando
        call VaciarTeclado
        call EsperarTecla
        mov  al, estado         ; 1 victoria / 2 derrota
jpFin:
        call DetenerSonido      ; al salir (ESC o fin) el menu espera tecla sin atender el sonido
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
        RELLENAR_RECT C_FONDO, 0, 160, 320, 8   ; borrar el aviso anterior
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
        mov  al, secreto[si]
        push cx
        mov  cx, dx
        mov  bx, 150
        mov  dl, C_CELDA
        call DibujarFichaSobre
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

        mov  ax, tiempoFinal    ; duracion de la partida (victoria o derrota)
        mov  di, OFFSET tiempoFinBuf + 8
        call FormatearTiempo
        CENTRAR tiempoFinBuf, 112, C_TEXTO, 1

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
        call DibujarTituloFin   ; la pantalla debe estar completa antes del fundido
        call VaciarTeclado
        mov  si, OFFSET sndVictoria     ; la melodia sube si ganaste...
        cmp  estado, ESTADO_VICTORIA
        je   pfSonar
        mov  si, OFFSET sndDerrota      ; ...y baja si perdiste
pfSonar:
        call IniciarSonido
pfEspera:
        call ActualizarSonido
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
        call DetenerSonido
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
PantallaFin ENDP