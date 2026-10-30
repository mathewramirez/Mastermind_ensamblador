;=====================================================================
; CONST.INC - Constantes del proyecto
; EQU no ocupa memoria: el ensamblador sustituye el valor. Tenerlas
; todas aqui permite cambiar una zona, el color transparente o la
; velocidad de animacion editando UNA sola linea.
;=====================================================================

; --- Video ---
VIDEO_SEG       EQU 0A000h      ; segmento de la memoria de video en modo 13h
ANCHO_PANT      EQU 320
ALTO_PANT       EQU 200
COLOR_TRANSP    EQU 0           ; indice de color que DibujarSprite NO escribe (RG-03)

; --- Zonas de pantalla (enunciado: origen x,y y tamano) ---
ENC_X           EQU 0           ; A. Encabezado
ENC_Y           EQU 0
ENC_W           EQU 320
ENC_H           EQU 20
TAB_X           EQU 4           ; B. Tablero
TAB_Y           EQU 22
TAB_W           EQU 196
TAB_H           EQU 168
PAN_X           EQU 204         ; C. Panel lateral
PAN_Y           EQU 22
PAN_W           EQU 112
PAN_H           EQU 168
EST_X           EQU 0           ; D. Barra de estado
EST_Y           EQU 190
EST_W           EQU 320
EST_H           EQU 10

; --- Tablero y fichas ---
FICHA_LADO      EQU 16          ; las fichas miden 16x16
FILA_ALTO       EQU 16          ; 10 filas de 16 px caben en los 168 px del tablero
MAX_N           EQU 5           ; maximo de posiciones (nivel dificil)
MAX_INTENTOS    EQU 12          ; maximo de intentos (nivel facil)

; --- Niveles (indice en tablaNiveles) ---
NIVEL_FACIL     EQU 0
NIVEL_NORMAL    EQU 1
NIVEL_DIFICIL   EQU 2

; --- Estado de la partida ---
ESTADO_JUGANDO  EQU 0
ESTADO_VICTORIA EQU 1
ESTADO_DERROTA  EQU 2

; --- Teclado: INT 16h AH=00h devuelve AH=scan code, AL=ASCII ---
; Las flechas son teclas extendidas: AL=0 (o E0h en algunos teclados)
; y lo que identifica la tecla es el scan code en AH.
SCAN_ARRIBA     EQU 48h
SCAN_IZQ        EQU 4Bh
SCAN_DER        EQU 4Dh
SCAN_ABAJO      EQU 50h
ASC_ENTER       EQU 0Dh
ASC_RETROCESO   EQU 08h
ASC_ESC         EQU 1Bh

; --- Acciones: lo que significa una tecla para el juego (TraducirTecla) ---
ACC_NINGUNA     EQU 0           ; tecla invalida: se ignora
ACC_IZQ         EQU 1           ; posicion activa a la izquierda
ACC_DER         EQU 2           ; posicion activa a la derecha
ACC_ARRIBA      EQU 3           ; simbolo siguiente en la posicion activa
ACC_ABAJO       EQU 4           ; simbolo anterior en la posicion activa
ACC_SIMBOLO     EQU 5           ; tecla 1..C: AH trae el ID del simbolo
ACC_BORRAR      EQU 6           ; BACKSPACE
ACC_CONFIRMAR   EQU 7           ; ENTER
ACC_SALIR       EQU 8           ; ESC

; --- Resultado de aplicar una accion al intento (ProcesarAccion) ---
RES_NADA        EQU 0           ; no cambio nada
RES_CAMBIO      EQU 1           ; cambio el intento o la posicion: redibujar
RES_INCOMPLETO  EQU 2           ; ENTER con casillas vacias: avisar, NO gastar turno
RES_CONFIRMAR   EQU 3           ; ENTER con el intento completo: evaluar
RES_SALIR       EQU 4           ; ESC

; --- Animacion: se mide en tics del BIOS (18.2 por segundo) ---
TICS_ANIM       EQU 4           ; un cuadro cada ~0.22 s


; --- Colores reutilizados de la paleta generada (ver doc/tabla_paleta.md) ---
C_DORADO        EQU 37          ; amarillo de spr_exacto: titulos y seleccion

; --- Opciones del menu principal (orden en que se dibujan) ---
OPC_JUGAR       EQU 0
OPC_NIVEL       EQU 1
OPC_INSTRUC     EQU 2
OPC_CREDITOS    EQU 3
OPC_SALIR       EQU 4
NUM_OPC_MENU    EQU 5


; --- Fase 6: colores de la paleta reutilizados y geometria de la partida ---
C_CELDA         EQU 50          ; arena (de spr_mascota): fondo claro tras cada ficha
C_ROJO          EQU 33          ; rojo vivo (de spr_derrota): titulo de derrota
CASILLA_LADO    EQU 20          ; casilla del intento en el panel: 20x20 px
EST_COLUMNAS    EQU 40          ; caracteres que caben en la barra de estado (320 px / 8)