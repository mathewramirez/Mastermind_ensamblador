
; MASTER.ASM - Mastermind "Simbolos de Costa Rica"

        .MODEL SMALL
        .286                    
        .STACK 200h

        INCLUDE CONST.INC       ; solo EQU, no genera codigo ni datos
        INCLUDE MACROS.INC      ; las macros 

        .DATA
        INCLUDE PALETA.INC      ; tablaPaleta + constantes C_FONDO, C_PANEL...
        INCLUDE SPRITES.INC     ; tablas de bytes de cada sprite (generadas)
        INCLUDE DATOS.INC       ; variables y tablas del juego
        INCLUDE TEXTOS.INC      ; cadenas de las pantallas

        .CODE
; Punto de entrada. DOS deja DS apuntando al PSP, no a .DATA.
inicio:
        mov  ax, @data
        mov  ds, ax
        cld                     ; LODSB/STOSB avanzan hacia adelante

        call GuardarModoVideo
        call EntrarModo13h
        call CargarPaleta       ; sin esto los indices 16..71 serian grises
        call CargarFuente       ; direccion de la fuente 8x8 del BIOS
        call CargarPuntajes     ; mejores puntajes guardados en MEJORES.DAT

        call PantallaPresentacion
menuPrincipal:
        call PantallaMenu       ; AL = opcion elegida
        cmp  al, OPC_JUGAR
        jne  noJugar
jugarOtra:
        cmp  modo2j, 0
        je   unJugador
        call PartidaDosJugadores ; dos rondas por turnos; AL = 1 otra vez, 0 menu
        or   al, al
        jz   menuPrincipal
        jmp  jugarOtra
unJugador:
        call IniciarPartida     ; configura el nivel y genera el secreto
        call JugarPartida       ; AL = 0 abandono, 1 victoria, 2 derrota
        or   al, al
        jz   menuPrincipal
        push ax                 ; AL = resultado, se necesita despues
        cmp  al, ESTADO_VICTORIA
        jne  sinRegistro
        call RegistrarPuntaje   ; si entra al top 5 pide las iniciales
sinRegistro:
        pop  ax
        call PantallaFin        ; AL = 1 jugar otra vez, 0 menu
        or   al, al
        jz   menuPrincipal
        jmp  jugarOtra
noJugar:
        cmp  al, OPC_NIVEL
        jne  noNivel
        call PantallaDificultad
        jmp  menuPrincipal
noNivel:
        cmp  al, OPC_INVERSO
        jne  noInverso
jugarInv:
        call PartidaInversa     ; el programa adivina; AL = 1 otra vez, 0 menu
        or   al, al
        jz   menuPrincipal
        jmp  jugarInv
noInverso:
        cmp  al, OPC_PUNTAJES
        jne  noPuntajes
        call PantallaPuntajes
        jmp  menuPrincipal
noPuntajes:
        cmp  al, OPC_INSTRUC
        jne  noInstruc
        call PantallaInstrucciones
        jmp  menuPrincipal
noInstruc:
        cmp  al, OPC_CREDITOS
        jne  salir              ; la unica que queda es OPC_SALIR
        call PantallaCreditos
        jmp  menuPrincipal

salir:
        call DetenerSonido      ; dejar el altavoz en silencio al volver a DOS
        call RestaurarModoVideo ; volver al modo de texto de DOS
        mov  ax, 4C00h          ; AH=4Ch terminar programa, AL=00 sin error
        int  21h

; Procedimientos, un archivo por responsabilidad.
        INCLUDE VIDEO.INC
        INCLUDE TEXTO.INC
        INCLUDE ANIM.INC
        INCLUDE CRONO.INC
        INCLUDE SONIDO.INC
        INCLUDE FADE.INC
        INCLUDE TECLADO.INC
        INCLUDE ALEAT.INC
        INCLUDE JUEGO.INC
        INCLUDE ENTRADA.INC
        INCLUDE CICLO.INC
        INCLUDE TABLERO.INC
        INCLUDE PANEL.INC
        INCLUDE PARTIDA.INC
        INCLUDE PANTALLA.INC
        INCLUDE PUNTAJES.INC
        INCLUDE DOSJUG.INC
        INCLUDE INVERSO.INC


        END inicio