
; DOSJUG.INC - Modo dos jugadores por turnos en la misma maquina (opcional)

; RondaDos
;   Recibe:    jugCreador (1 o 2): quien escribe el secreto esta ronda
;   Devuelve:  AL = 0 si el que adivinaba abandono con ESC; AL = 1 si la
;              ronda termino y su resultado quedo en dosInt / dosTiempo
;   Destruye:  AX
;   El que adivina es el otro jugador: su indice (0 o 1) es 2 - jugCreador.
RondaDos PROC
        push bx

        call IniciarPartida
        call JugarPartida       ; AL = 0 abandono, 1 victoria, 2 derrota
        or   al, al
        jz   rjFin
        mov  bl, 2
        sub  bl, jugCreador     ; BL = indice del jugador que adivino
        xor  bh, bh
        cmp  al, ESTADO_VICTORIA
        jne  rjPerdio
        mov  al, turno          ; gano: cuenta los intentos usados
        mov  dosInt[bx], al
        jmp  rjTiempo
rjPerdio:
        mov  dosInt[bx], 0FFh   ; no adivino: peor que cualquier resultado
rjTiempo:
        shl  bx, 1              ; dosTiempo son palabras
        mov  ax, tiempoFinal
        mov  dosTiempo[bx], ax
        mov  al, 1
rjFin:
        pop  bx
        ret
RondaDos ENDP

; CompararDos
;   Recibe:    dosInt[], dosTiempo[]
;   Devuelve:  AL = 1 gana el jugador 1, 2 gana el jugador 2, 0 empate
;              (mismo resultado, o ninguno adivino)
;   Destruye:  AX
CompararDos PROC
        mov  al, dosInt
        mov  ah, dosInt + 1
        cmp  al, ah
        jb   cdUno              ; el 1 uso menos intentos
        ja   cdDos
        cmp  al, 0FFh
        je   cdEmpate           ; ninguno adivino
        mov  ax, dosTiempo
        cmp  ax, dosTiempo + 2
        jb   cdUno              ; mismos intentos: gana el mas rapido
        ja   cdDos
cdEmpate:
        xor  al, al
        ret
cdUno:
        mov  al, 1
        ret
cdDos:
        mov  al, 2
        ret
CompararDos ENDP

; DibujarFilaDos
;   Recibe:    AL = jugador 0 o 1
;   Devuelve:  "Jugador n:  k intento(s)  mm:ss" o "Jugador n:  no adivino"
;              en la fila de ese jugador (y = 112 + 16 * indice)
;   Destruye:  nada
DibujarFilaDos PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        xor  ah, ah
        mov  di, ax             ; DI = indice del jugador
        mov  bl, 16
        mul  bl
        add  ax, 112
        mov  puntY, ax          ; y de esta fila
        mov  ax, di
        add  al, '1'
        mov  tResJ + 8, al      ; "Jugador 1:" o "Jugador 2:"
        TEXTO tResJ, 52, puntY, C_TEXTO, 1
        cmp  dosInt[di], 0FFh
        je   frNoAdivino

        mov  al, dosInt[di]
        mov  cx, 148
        mov  bx, puntY
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarNumero      ; intentos
        TEXTO tResInt, 172, puntY, C_TEXTO, 1
        mov  bx, di
        shl  bx, 1
        mov  ax, dosTiempo[bx]
        mov  di, OFFSET tiempoPunt
        call FormatearTiempo    ; AX segundos -> "mm:ss" en tiempoPunt
        TEXTO tiempoPunt, 260, puntY, C_TEXTO, 1
        jmp  frFin
frNoAdivino:
        TEXTO tResNo, 148, puntY, C_ROJO, 1
frFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarFilaDos ENDP

; PantallaResultadoDos
;   Recibe:    dosInt[], dosTiempo[]
;   Devuelve:  AL = 1 jugar otra vez (ENTER), 0 volver al menu (ESC/M)
;   Destruye:  AX
;   Muestra quien gano (con la animacion de victoria y su melodia) o el
;   empate, y el resultado de cada jugador.
PantallaResultadoDos PROC
        push bx
        push cx
        push dx
        push si
        push di

        call CompararDos
        mov  cl, al             ; CL = 0 empate, 1 o 2 = ganador
        mov  si, OFFSET tHdrResDos
        mov  di, OFFSET tBarraFin
        call PintarPantalla

        mov  si, OFFSET MASCOTA_0
        mov  di, OFFSET MASCOTA_1
        or   cl, cl
        jz   prAnim             ; empate: mascota
        mov  si, OFFSET VICTORIA_0
        mov  di, OFFSET VICTORIA_1
prAnim:
        mov  ax, 144
        mov  bx, 30
        mov  dl, C_FONDO
        call IniciarAnimacion

        or   cl, cl
        jnz  prGana
        CENTRAR tEmpate, 72, C_DORADO, 2
        jmp  prFilas
prGana:
        mov  al, cl
        add  al, '0'
        mov  tGanaJ + 16, al    ; "GANA EL JUGADOR 1" o "...2"
        CENTRAR tGanaJ, 72, C_DORADO, 2
prFilas:
        xor  al, al
        call DibujarFilaDos
        mov  al, 1
        call DibujarFilaDos
        cmp  puntPuesto, 0FFh
        je   prSinRecord        ; solo se anuncia si el ganador entro al top 5
        TEXTO tRecordFin, 72, 150, C_DORADO, 1
        mov  al, puntPuesto
        inc  al                 ; puesto desde 1
        mov  cx, 240
        mov  bx, 150
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarNumero
prSinRecord:

        call VaciarTeclado
        or   cl, cl
        jz   prEspera           ; en empate no suena la melodia
        mov  si, OFFSET sndVictoria
        call IniciarSonido
prEspera:
        call ActualizarSonido
        call ActualizarMascota
        call HayTecla
        jz   prEspera
        call EsperarTecla
        cmp  al, ASC_ENTER
        je   prOtra
        cmp  al, ASC_ESC
        je   prMenu
        or   al, 20h            ; a minuscula
        cmp  al, 'm'
        jne  prEspera
prMenu:
        xor  al, al
        jmp  prFin
prOtra:
        mov  al, 1
prFin:
        call DetenerSonido
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
PantallaResultadoDos ENDP

; PartidaDosJugadores
;   Recibe:    nivel, modo2j = 1
;   Devuelve:  AL = 1 jugar otra vez, 0 volver al menu (tambien si alguien
;              abandona con ESC durante una ronda)
;   Destruye:  AX
PartidaDosJugadores PROC
        mov  jugCreador, 2      ; ronda 1: juega (adivina) el jugador 1; el 2 escribe
        call RondaDos
        or   al, al
        jz   pjFin
        mov  jugCreador, 1      ; ronda 2: juega el jugador 2; el 1 escribe
        call RondaDos
        or   al, al
        jz   pjFin
        call RegistrarDos       ; el ganador entra a Mejores puntajes si le toca
        call PantallaResultadoDos
pjFin:
        ret
PartidaDosJugadores ENDP
