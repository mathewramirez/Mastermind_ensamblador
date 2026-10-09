;=====================================================================
; MASTER.ASM - Mastermind "Simbolos de Costa Rica"
; EIF205 Arquitectura de Computadores - Proyecto II - II Ciclo 2026
; UNA Sede Regional Brunca
;
; PASO 2: paleta personalizada + rutina generica de sprites.
;   - CargarPaleta redefine los colores 16..52 por los puertos 3C8h/3C9h
;   - LimpiarPantalla llena la VRAM de un color con REP STOSW
;   - DibujarSprite dibuja CUALQUIER sprite, saltando el color 0
;
; Ensamblar (desde Windows, en la raiz del proyecto):
;       BUILD.bat src\MASTER
;=====================================================================

        .MODEL SMALL            ; 1 segmento de codigo + 1 de datos (64 KB c/u)
        .286                    ; permite "shl reg, 8" y "push inmediato"
        .STACK 200h             ; 512 bytes de pila

;---------------------------------------------------------------------
; Constantes (EQU no ocupa memoria: el ensamblador las sustituye)
;---------------------------------------------------------------------
VIDEO_SEG       EQU 0A000h      ; segmento donde vive la pantalla en modo 13h
ANCHO_PANT      EQU 320
ALTO_PANT       EQU 200

        INCLUDE MACROS.INC      ; las macros deben definirse ANTES de usarse

        .DATA
modoOriginal    DB ?            ; modo de video de DOS, para dejarlo igual al salir

        INCLUDE PALETA.INC      ; tablaPaleta + constantes C_FONDO, C_PANEL...
        INCLUDE SPRITES.INC     ; tablas de bytes de cada sprite (generadas)

        .CODE
;=====================================================================
; Punto de entrada. DOS nos deja DS apuntando al PSP, no a .DATA,
; por eso lo primero es cargar DS con nuestro segmento de datos.
;=====================================================================
inicio:
        mov  ax, @data
        mov  ds, ax
        cld                     ; LODSB/STOSW avanzan hacia adelante (SI++, DI++)

        call GuardarModoVideo
        call EntrarModo13h
        call CargarPaleta       ; sin esto los indices 16..52 serian grises del VGA

        mov  al, C_FONDO
        call LimpiarPantalla

        ; --- Las 8 fichas en fila, separadas 36 px, centradas ---
        DIBUJAR_SPRITE CAFE_0,           CAFE_ANCHO,           CAFE_ALTO,            20, 92
        DIBUJAR_SPRITE CARRETA_0,        CARRETA_ANCHO,        CARRETA_ALTO,         56, 92
        DIBUJAR_SPRITE GUARIAMORADA_0,   GUARIAMORADA_ANCHO,   GUARIAMORADA_ALTO,    92, 92
        DIBUJAR_SPRITE MARIMBA_0,        MARIMBA_ANCHO,        MARIMBA_ALTO,        128, 92
        DIBUJAR_SPRITE MARIPOSAMORPHO_0, MARIPOSAMORPHO_ANCHO, MARIPOSAMORPHO_ALTO, 164, 92
        DIBUJAR_SPRITE TORTUGA_0,        TORTUGA_ANCHO,        TORTUGA_ALTO,        200, 92
        DIBUJAR_SPRITE VOLCAN_0,         VOLCAN_ANCHO,         VOLCAN_ALTO,         236, 92
        DIBUJAR_SPRITE YIGUIRRO_0,       YIGUIRRO_ANCHO,       YIGUIRRO_ALTO,       272, 92

        call EsperarTecla
        call RestaurarModoVideo

        mov  ax, 4C00h          ; AH=4Ch terminar programa, AL=00 codigo de salida
        int  21h

;---------------------------------------------------------------------
; Procedimientos, un archivo por responsabilidad.
;---------------------------------------------------------------------
        INCLUDE VIDEO.INC
        INCLUDE TECLADO.INC

        END inicio
