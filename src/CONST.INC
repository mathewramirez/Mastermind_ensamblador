; CONST.INC - Constantes del proyecto



;  Video
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

;  Tablero y fichas
FICHA_LADO      EQU 16          ; las fichas miden 16x16
FILA_ALTO       EQU 16          ; 10 filas de 16 px caben en los 168 px del tablero
MAX_N           EQU 5           ; maximo de posiciones (nivel dificil)
MAX_INTENTOS    EQU 12          ; maximo de intentos (nivel facil)

;  Niveles (indice en tablaNiveles) 
NIVEL_FACIL     EQU 0
NIVEL_NORMAL    EQU 1
NIVEL_DIFICIL   EQU 2

; Estado de la partida
ESTADO_JUGANDO  EQU 0
ESTADO_VICTORIA EQU 1
ESTADO_DERROTA  EQU 2

; Teclado: INT 16h AH=00h devuelve AH=scan code, AL=ASCII 
SCAN_ARRIBA     EQU 48h
SCAN_IZQ        EQU 4Bh
SCAN_DER        EQU 4Dh
SCAN_ABAJO      EQU 50h
ASC_ENTER       EQU 0Dh
ASC_RETROCESO   EQU 08h
ASC_ESC         EQU 1Bh

; Acciones: lo que significa una tecla para el juego (TraducirTecla)
ACC_NINGUNA     EQU 0           ; tecla invalida: se ignora
ACC_IZQ         EQU 1           ; posicion activa a la izquierda
ACC_DER         EQU 2           ; posicion activa a la derecha
ACC_ARRIBA      EQU 3           ; simbolo siguiente en la posicion activa
ACC_ABAJO       EQU 4           ; simbolo anterior en la posicion activa
ACC_SIMBOLO     EQU 5           ; tecla 1..C: AH trae el ID del simbolo
ACC_BORRAR      EQU 6           ; BACKSPACE
ACC_CONFIRMAR   EQU 7           ; ENTER
ACC_SALIR       EQU 8           ; ESC

; Resultado de aplicar una accion al intento (ProcesarAccion)
RES_NADA        EQU 0           
RES_CAMBIO      EQU 1          
RES_INCOMPLETO  EQU 2           
RES_CONFIRMAR   EQU 3          
RES_SALIR       EQU 4           

;  Animacion: se mide en tics del BIOS (18.2 por segundo) 
TICS_ANIM       EQU 4           ; un cuadro cada ~0.22 s

;  Colores reutilizados de la paleta generada 
C_DORADO        EQU 37          ; amarillo de spr_exacto: titulos y seleccion

;  Opciones del menu principal (orden en que se dibujan)
OPC_JUGAR       EQU 0
OPC_NIVEL       EQU 1
OPC_JUGADORES   EQU 2           ; no abre pantalla: alterna 1 o 2 jugadores
OPC_INVERSO     EQU 3           ; modo inverso: el programa adivina tu codigo
OPC_PUNTAJES    EQU 4
OPC_INSTRUC     EQU 5
OPC_CREDITOS    EQU 6
OPC_SALIR       EQU 7
NUM_OPC_MENU    EQU 8

;  colores de la paleta reutilizados y geometria de la partida
C_CELDA         EQU 50          
C_ROJO          EQU 33          ; rojo vivo 
CASILLA_LADO    EQU 20          
EST_COLUMNAS    EQU 40          

; Extra: mejores puntajes (archivo MEJORES.DAT)
PUNT_REC        EQU 6          
PUNT_TOP        EQU 5           
PUNT_NIVEL      EQU PUNT_REC * PUNT_TOP     ; 30 bytes por nivel
PUNT_CAT        EQU PUNT_NIVEL * 3          ; 90 bytes por categoria (3 niveles)
PUNT_CATS       EQU 3                       ; 0 = 1 jugador, 1 = 2 jugadores, 2 = computadora
PUNT_TOTAL      EQU PUNT_CAT * PUNT_CATS    ; 270 bytes en total
PUNT_ARCH       EQU 2 + PUNT_TOTAL          ; 272 bytes: firma + tablas
PUNT_ARCH_V1    EQU 2 + PUNT_CAT            ; 92 bytes: archivo de la version anterior (solo 1 jugador)
NOM_X           EQU 124         ; pantalla de iniciales: x de la primera casilla
NOM_Y           EQU 114         ; y de las casillas
NUM_PAG_INSTR   EQU 5          ; paginas de la pantalla de instrucciones