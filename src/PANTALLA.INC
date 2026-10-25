
;=====================================================================
; PANTALLA.INC - Presentacion, menu, dificultad, instrucciones,
; creditos y pantalla provisional de partida (Fase 5, RF/Pantallas A-D,G)
;
; Todas siguen la distribucion del enunciado: encabezado (0,0,320x20),
; area de contenido, barra de estado (0,190,320x10). Cada pantalla
; pinta el fondo UNA vez y luego redibuja solo la zona que cambia.
;=====================================================================

;---------------------------------------------------------------------
; PintarPantalla  (apaga la pantalla anterior con un fundido; ver FADE.INC)
;   Recibe:    SI = texto del encabezado, DI = texto de la barra de estado
;   Devuelve:  fondo, encabezado y barra de estado dibujados
;   Destruye:  nada
;---------------------------------------------------------------------
PintarPantalla PROC
        push ax
        push bx
        push dx
        push si

        call FundidoSalida      ; la pantalla anterior se apaga...
        mov  fundidoPend, 1     ; ...y la nueva aparece en la primera lectura de tecla
        mov  al, C_FONDO
        call LimpiarPantalla
        RELLENAR_RECT C_PANEL, ENC_X, ENC_Y, ENC_W, ENC_H
        RELLENAR_RECT C_BORDE, 0, 19, 320, 1
        RELLENAR_RECT C_PANEL, EST_X, EST_Y, EST_W, EST_H
        RELLENAR_RECT C_BORDE, 0, 189, 320, 1

        mov  bx, 6              ; titulo centrado dentro de los 20 px del encabezado
        mov  dl, C_DORADO
        mov  dh, 1
        call CentrarTexto       ; SI ya es el texto del encabezado
        mov  si, di
        mov  bx, 191
        mov  dl, C_TEXTO
        mov  dh, 1
        call CentrarTexto

        pop  si
        pop  dx
        pop  bx
        pop  ax
        ret
PintarPantalla ENDP

;---------------------------------------------------------------------
; DibujarFilaFichas
;   Recibe:    AX = x de la primera, BX = y, CX = cuantas fichas,
;              DX = separacion horizontal entre fichas (en pixeles)
;   Devuelve:  las primeras CX fichas de tablaFichas dibujadas en fila
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarFilaFichas PROC
        push ax
        push cx
        push si
        xor  si, si             ; SI = desplazamiento en tablaFichas (2 bytes c/u)
dfSiguiente:
        push tablaFichas[si]
        push FICHA_LADO
        push FICHA_LADO
        push ax
        push bx
        call DibujarSprite      ; conserva AX, BX, CX, DX
        add  ax, dx
        add  si, 2
        loop dfSiguiente
        pop  si
        pop  cx
        pop  ax
        ret
DibujarFilaFichas ENDP

;---------------------------------------------------------------------
; MoverSeleccion
;   Recibe:    AL = accion (ACC_ARRIBA o ACC_ABAJO), DL = seleccion actual,
;              DH = cantidad de opciones
;   Devuelve:  DL = nueva seleccion (da la vuelta en los extremos);
;              con cualquier otra accion DL no cambia
;   Destruye:  nada
;---------------------------------------------------------------------
MoverSeleccion PROC
        cmp  al, ACC_ARRIBA
        jne  msAbajo
        or   dl, dl
        jnz  msRestar
        mov  dl, dh             ; estaba en la primera: pasar a la ultima
msRestar:
        dec  dl
        ret
msAbajo:
        cmp  al, ACC_ABAJO
        jne  msFin
        inc  dl
        cmp  dl, dh
        jb   msFin
        xor  dl, dl             ; paso la ultima: volver a la primera
msFin:
        ret
MoverSeleccion ENDP

;---------------------------------------------------------------------
; DibujarNivelActual
;   Recibe:    nivel
;   Devuelve:  "Nivel actual: <nombre>" dibujado en (88,172)
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarNivelActual PROC
        push ax
        push bx
        push dx
        push si
        TEXTO tNivelAct, 88, 172, C_TEXTO, 1
        mov  bl, nivel
        xor  bh, bh
        shl  bx, 1
        mov  si, nombresNivel[bx]
        mov  ax, 200            ; 88 + 13 caracteres*8 + 8 de espacio
        mov  bx, 172
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarTexto
        pop  si
        pop  dx
        pop  bx
        pop  ax
        ret
DibujarNivelActual ENDP

;=====================================================================
; A. PRESENTACION
;=====================================================================

;---------------------------------------------------------------------
; PantallaPresentacion
;   Recibe:    nada
;   Devuelve:  vuelve cuando el jugador pulsa una tecla; mientras tanto
;              la mascota alterna sus 2 cuadros (RG-05)
;   Destruye:  nada
;---------------------------------------------------------------------
PantallaPresentacion PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        mov  si, OFFSET tHdrPres
        mov  di, OFFSET tBarraPres
        call PintarPantalla

        CENTRAR tTitulo, 28, C_DORADO, 3
        CENTRAR tSubtitulo, 58, C_TEXTO, 1
        mov  ax, 68             ; 8 fichas de 16 px con 8 px de separacion
        mov  bx, 74
        mov  cx, 8
        mov  dx, 24
        call DibujarFilaFichas

        CENTRAR tIntegr, 136, C_DORADO, 1
        CENTRAR tAutor1, 146, C_TEXTO, 1
        CENTRAR tAutor2, 156, C_TEXTO, 1
        CENTRAR tCurso1, 170, C_TEXTO, 1
        CENTRAR tCurso2, 180, C_TEXTO, 1

        mov  si, OFFSET MASCOTA_0
        mov  di, OFFSET MASCOTA_1
        mov  ax, 144
        mov  bx, 98
        mov  dl, C_FONDO
        call IniciarAnimacion
        call VaciarTeclado
        call EsperarTeclaAnimada

        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
PantallaPresentacion ENDP

;=====================================================================
; B. MENU PRINCIPAL
;=====================================================================

;---------------------------------------------------------------------
; DibujarOpcion
;   Recibe:    AL = opcion (0..4), opcionMenu (la resaltada)
;   Devuelve:  solo ESA fila del menu: barra (azul si esta resaltada,
;              fondo si no), su texto y, si esta resaltada, los triangulos
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarOpcion PROC
        push ax
        push bx
        push dx
        push si
        push di

        xor  ah, ah
        mov  di, ax             ; DI = opcion
        mov  bl, 26
        mul  bl                 ; AX = i * 26
        add  ax, 28
        mov  bx, ax             ; BX = y de la barra de esta opcion
        mov  dl, C_FONDO        ; sin resaltar: la barra se borra con el fondo
        mov  al, opcionMenu
        xor  ah, ah
        cmp  ax, di
        jne  doBarra
        mov  dl, C_BORDE        ; resaltada: barra azul
doBarra:
        RELLENAR_RECT dx, 40, bx, 240, 22
        mov  si, di
        shl  si, 1
        mov  si, opcionesMenu[si]
        add  bx, 3              ; el texto de 16 px queda centrado en la barra
        mov  dl, C_TEXTO
        mov  dh, 2
        call CentrarTexto
        mov  al, opcionMenu
        xor  ah, ah
        cmp  ax, di
        jne  doFin
        TEXTO tFlechaIzq, 48, bx, C_DORADO, 2
        TEXTO tFlechaDer, 264, bx, C_DORADO, 2
doFin:
        pop  di
        pop  si
        pop  dx
        pop  bx
        pop  ax
        ret
DibujarOpcion ENDP

;---------------------------------------------------------------------
; DibujarOpcionesMenu
;   Recibe:    opcionMenu (opcion resaltada), nivel; la pantalla ya
;              esta pintada con PintarPantalla
;   Devuelve:  las 5 opciones, el nivel actual y el aviso de modo prueba
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarOpcionesMenu PROC
        push ax
        push cx
        xor  cl, cl             ; CL = opcion que se dibuja (0..4)
doItem:
        mov  al, cl
        call DibujarOpcion
        inc  cl
        cmp  cl, NUM_OPC_MENU
        jb   doItem
        call DibujarNivelActual
        call DibujarModoPrueba
        pop  cx
        pop  ax
        ret
DibujarOpcionesMenu ENDP

;---------------------------------------------------------------------
; PantallaMenu
;   Recibe:    opcionMenu (la ultima que se uso)
;   Devuelve:  AL = opcion elegida con ENTER (OPC_xxx)
;   Destruye:  AX
;   Navegacion: flechas arriba/abajo; las demas teclas se ignoran.
;---------------------------------------------------------------------
PantallaMenu PROC
        push dx
        push si
        push di

        mov  si, OFFSET tHdrMenu
        mov  di, OFFSET tBarraMenu
        call PintarPantalla
        call DibujarOpcionesMenu
        call VaciarTeclado
pmLeer:
        call EsperarTecla
        cmp  al, 't'            ; T activa/desactiva el modo prueba (secreto manual)
        je   pmPrueba
        cmp  al, 'T'
        je   pmPrueba
        call TraducirTecla      ; AL = accion; mismo codigo que la partida
        cmp  al, ACC_CONFIRMAR
        je   pmElegida
        cmp  al, ACC_ARRIBA
        je   pmMover
        cmp  al, ACC_ABAJO
        jne  pmLeer             ; cualquier otra tecla: ignorar
pmMover:
        mov  dl, opcionMenu
        mov  dh, NUM_OPC_MENU
        push dx                 ; DL = opcion que se apaga
        call MoverSeleccion
        mov  opcionMenu, dl
        pop  ax                 ; AL = opcion anterior
        call DibujarOpcion      ; solo se redibujan las 2 filas que cambian
        mov  al, opcionMenu
        call DibujarOpcion
        jmp  pmLeer
pmPrueba:
        xor  modoPrueba, 1
        call DibujarModoPrueba
        jmp  pmLeer
pmElegida:
        mov  al, opcionMenu
        pop  di
        pop  si
        pop  dx
        ret
PantallaMenu ENDP

;=====================================================================
; C. SELECCION DE DIFICULTAD
;=====================================================================

; DibujarTarjeta
;   Recibe:    AL = nivel (0..2), selNivel (el resaltado)
;   Devuelve:  la tarjeta de ese nivel con sus reglas; la resaltada
;              tiene borde dorado
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarTarjeta PROC
        push ax
        push bx
        push dx
        push si
        push di

        xor  ah, ah
        mov  di, ax             ; DI = nivel que se dibuja (0..2)
        mov  bl, 52
        mul  bl
        add  ax, 26
        mov  bx, ax             ; BX = y de la tarjeta
        mov  dl, C_BORDE
        mov  al, selNivel
        xor  ah, ah
        cmp  ax, di
        jne  dkMarco
        mov  dl, C_DORADO       ; el resaltado se ve por el borde dorado
dkMarco:
        RELLENAR_RECT dx, 12, bx, 296, 46
        RELLENAR_RECT C_PANEL, 14, bx, 292, 44
        mov  si, di
        shl  si, 1
        push si
        mov  si, nombresNivel[si]
        mov  ax, 24
        add  bx, 4
        mov  dl, C_TEXTO
        mov  dh, 2
        call DibujarTexto
        pop  si
        add  bx, 20
        push si
        mov  si, detalle1[si]
        mov  ax, 24
        mov  dl, C_TEXTO
        mov  dh, 1
        call DibujarTexto
        pop  si
        add  bx, 10
        mov  si, detalle2[si]
        mov  ax, 24
        call DibujarTexto

        pop  di
        pop  si
        pop  dx
        pop  bx
        pop  ax
        ret
DibujarTarjeta ENDP

; DibujarBordeTarjeta
;   Recibe:    AL = nivel (0..2), selNivel (el resaltado)
;   Devuelve:  SOLO el borde de la tarjeta (dorado si esta resaltada, azul si
;              no). El interior y el texto no se tocan, asi al mover la
;              seleccion no hay ningun instante con la tarjeta en blanco.
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarBordeTarjeta PROC
        push ax
        push bx
        push cx
        push dx

        xor  ah, ah
        mov  cx, ax             ; CX = nivel de esta tarjeta
        mov  bl, 52
        mul  bl
        add  ax, 26
        mov  bx, ax             ; BX = y de la tarjeta
        mov  dl, C_BORDE
        mov  al, selNivel
        xor  ah, ah
        cmp  ax, cx
        jne  dbPintar
        mov  dl, C_DORADO       ; la resaltada lleva el borde dorado
dbPintar:
        RELLENAR_RECT dx, 12, bx, 2, 46      ; lado izquierdo
        RELLENAR_RECT dx, 306, bx, 2, 46     ; lado derecho
        add  bx, 44
        RELLENAR_RECT dx, 12, bx, 296, 2     ; lado inferior

        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarBordeTarjeta ENDP

;---------------------------------------------------------------------
; DibujarNiveles
;   Recibe:    selNivel (resaltado)
;   Devuelve:  las 3 tarjetas de nivel
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarNiveles PROC
        push ax
        push cx
        xor  cl, cl             ; CL = nivel
dnTarjeta:
        mov  al, cl
        call DibujarTarjeta
        inc  cl
        cmp  cl, 3
        jb   dnTarjeta
        pop  cx
        pop  ax
        ret
DibujarNiveles ENDP

;---------------------------------------------------------------------
; PantallaDificultad
;   Recibe:    nivel (nivel vigente)
;   Devuelve:  ENTER guarda el nivel resaltado en 'nivel'; ESC vuelve
;              sin cambiarlo
;   Destruye:  nada
;---------------------------------------------------------------------
PantallaDificultad PROC
        push ax
        push dx
        push si
        push di

        mov  al, nivel
        mov  selNivel, al
        mov  si, OFFSET tHdrNivel
        mov  di, OFFSET tBarraNivel
        call PintarPantalla
        call DibujarNiveles
        call VaciarTeclado
pdLeer:
        call EsperarTecla
        call TraducirTecla
        cmp  al, ACC_CONFIRMAR
        je   pdGuardar
        cmp  al, ACC_SALIR
        je   pdFin
        cmp  al, ACC_ARRIBA
        je   pdMover
        cmp  al, ACC_ABAJO
        jne  pdLeer
pdMover:
        mov  dl, selNivel
        mov  dh, 3
        push dx                 ; DL = tarjeta que se apaga
        call MoverSeleccion
        mov  selNivel, dl
        pop  ax                 ; AL = tarjeta anterior
        call DibujarBordeTarjeta ; solo cambia el borde de las 2 tarjetas implicadas
        mov  al, selNivel
        call DibujarBordeTarjeta
        jmp  pdLeer
pdGuardar:
        mov  al, selNivel
        mov  nivel, al          ; esta es la configuracion que usara IniciarPartida
pdFin:
        pop  di
        pop  si
        pop  dx
        pop  ax
        ret
PantallaDificultad ENDP

;=====================================================================
; D. INSTRUCCIONES (2 paginas)
;=====================================================================

;---------------------------------------------------------------------
; PaginaInstr1
;   Recibe: nada.  Devuelve: pagina 1 (objetivo y pistas, con iconos).
;   Destruye: nada
;---------------------------------------------------------------------
PaginaInstr1 PROC
        push ax
        push bx
        push dx
        push si
        TEXTO tObjetivo, 16, 28, C_DORADO, 1
        TEXTO tObj1,     16, 40, C_TEXTO, 1
        TEXTO tObj2,     16, 50, C_TEXTO, 1
        TEXTO tObj3,     16, 60, C_TEXTO, 1

        DIBUJAR_SPRITE EXACTO_0, EXACTO_ANCHO, EXACTO_ALTO, 16, 84
        TEXTO tExacto1,  32, 80, C_TEXTO, 1
        TEXTO tExacto2,  32, 90, C_TEXTO, 1

        DIBUJAR_SPRITE PARCIAL_0, PARCIAL_ANCHO, PARCIAL_ALTO, 16, 112
        TEXTO tParcial1, 32, 108, C_TEXTO, 1
        TEXTO tParcial2, 32, 118, C_TEXTO, 1

        TEXTO tNinguna,  16, 136, C_TEXTO, 1
        TEXTO tEjemplo,  16, 156, C_TEXTO, 1
        TEXTO tEjemplo2, 16, 166, C_TEXTO, 1
        pop  si
        pop  dx
        pop  bx
        pop  ax
        ret
PaginaInstr1 ENDP

;---------------------------------------------------------------------
; PaginaInstr2
;   Recibe: nada.  Devuelve: pagina 2 (niveles y controles).
;   Destruye: nada
;---------------------------------------------------------------------
PaginaInstr2 PROC
        push ax
        push bx
        push dx
        push si
        TEXTO tNiveles,   16, 28, C_DORADO, 1
        TEXTO tNivF1,     16, 40, C_TEXTO, 1
        TEXTO tNivF2,     16, 50, C_TEXTO, 1
        TEXTO tNivN1,     16, 62, C_TEXTO, 1
        TEXTO tNivN2,     16, 72, C_TEXTO, 1
        TEXTO tNivD1,     16, 84, C_TEXTO, 1
        TEXTO tNivD2,     16, 94, C_TEXTO, 1
        TEXTO tControles, 16, 112, C_DORADO, 1
        TEXTO tCtl1,      16, 124, C_TEXTO, 1
        TEXTO tCtl2,      16, 134, C_TEXTO, 1
        TEXTO tCtl3,      16, 144, C_TEXTO, 1
        TEXTO tCtl4,      16, 154, C_TEXTO, 1
        pop  si
        pop  dx
        pop  bx
        pop  ax
        ret
PaginaInstr2 ENDP

;---------------------------------------------------------------------
; PantallaInstrucciones
;   Recibe:    nada
;   Devuelve:  vuelve al menu con ESC. ENTER / flecha derecha pasan a la
;              pagina siguiente; flecha izquierda, a la anterior.
;   Destruye:  nada
;---------------------------------------------------------------------
PantallaInstrucciones PROC
        push ax
        push si
        push di

        mov  pagInstruc, 0
piRedibujar:
        mov  si, OFFSET tHdrInstr
        mov  di, OFFSET tBarraI1
        cmp  pagInstruc, 0
        je   piPintar
        mov  di, OFFSET tBarraI2
piPintar:
        call PintarPantalla
        cmp  pagInstruc, 0
        jne  piSegunda
        call PaginaInstr1
        jmp  piTecla
piSegunda:
        call PaginaInstr2
piTecla:
        call VaciarTeclado
piLeer:
        call EsperarTecla
        call TraducirTecla
        cmp  al, ACC_SALIR
        je   piFin
        cmp  al, ACC_DER
        je   piSiguiente
        cmp  al, ACC_CONFIRMAR
        je   piSiguiente
        cmp  al, ACC_IZQ
        jne  piLeer
        mov  pagInstruc, 0      ; izquierda: pagina anterior
        jmp  piRedibujar
piSiguiente:
        cmp  pagInstruc, 1
        je   piLeer             ; ya es la ultima pagina
        mov  pagInstruc, 1
        jmp  piRedibujar
piFin:
        pop  di
        pop  si
        pop  ax
        ret
PantallaInstrucciones ENDP

;=====================================================================
; G. CREDITOS
;=====================================================================

;---------------------------------------------------------------------
; PantallaCreditos
;   Recibe: nada.  Devuelve: vuelve al pulsar una tecla.  Destruye: nada
;---------------------------------------------------------------------
PantallaCreditos PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        mov  si, OFFSET tHdrCred
        mov  di, OFFSET tBarraVolver
        call PintarPantalla
        CENTRAR tTitulo, 28, C_DORADO, 3
        CENTRAR tCred1, 60, C_TEXTO, 1
        mov  ax, 68             ; las 8 fichas centradas (8x16 + 7x8 = 184 px)
        mov  bx, 76
        mov  cx, 8
        mov  dx, 24
        call DibujarFilaFichas
        CENTRAR tCred3, 104, C_DORADO, 1
        CENTRAR tAutor1, 116, C_TEXTO, 1
        CENTRAR tAutor2, 126, C_TEXTO, 1
        CENTRAR tCred4, 144, C_TEXTO, 1
        CENTRAR tCred5, 154, C_TEXTO, 1
        CENTRAR tCred6, 164, C_TEXTO, 1
        CENTRAR tCred7, 174, C_TEXTO, 1
        call VaciarTeclado
        call EsperarTecla

        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
PantallaCreditos ENDP

;=====================================================================
; PARTIDA (provisional: el tablero llega en la Fase 6)