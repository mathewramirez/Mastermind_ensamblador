;=====================================================================
; DATOS.INC - Variables y tablas del juego (van dentro de .DATA)
; Se incluye DESPUES de SPRITES.INC porque tablaFichas usa sus etiquetas.
;
; Vocabulario del tema "Simbolos de Costa Rica":
;   exacto  = simbolo correcto en la posicion correcta  ("toro" del enunciado)
;   parcial = simbolo correcto en otra posicion          ("vaca" del enunciado)
;=====================================================================

modoOriginal    DB ?            ; modo de video de DOS, para restaurarlo al salir

; --- Configuracion del nivel activo ---
nivel           DB NIVEL_NORMAL ; nivel elegido en el menu
numPos          DB 4            ; N: posiciones del codigo
numColores      DB 6            ; C: simbolos disponibles
repeticion      DB 1            ; 1 = el secreto puede repetir simbolos
maxIntentos     DB 10

; Una fila de 4 bytes por nivel: N, C, repeticion, intentos (Tabla 1)
; Se lee con indice: BX = nivel*4  ->  tablaNiveles[BX], [BX+1], ...
tablaNiveles    DB 4, 6, 0, 12  ; facil
                DB 4, 6, 1, 10  ; normal
                DB 5, 8, 1, 10  ; dificil

; --- Estado de la partida ---
; Los simbolos se guardan como ID 1..C. El 0 significa "casilla vacia",
; y EvaluarIntento lo usa para marcar fichas ya contadas.
secreto         DB MAX_N DUP(0) ; codigo a adivinar
intento         DB MAX_N DUP(0) ; intento que el jugador esta armando
copiaS          DB MAX_N DUP(0) ; copia de trabajo del secreto (EvaluarIntento la destruye)
copiaI          DB MAX_N DUP(0) ; copia de trabajo del intento

; Historial por filas: el simbolo j del intento t esta en historial[t*MAX_N + j]
historial       DB (MAX_INTENTOS*MAX_N) DUP(0)
histExactos     DB MAX_INTENTOS DUP(0)  ; aciertos exactos de cada intento
histParciales   DB MAX_INTENTOS DUP(0)  ; aciertos parciales de cada intento

turno           DB 0            ; intento en curso (empieza en 0)
posActiva       DB 0            ; posicion seleccionada del intento
colorSel        DB 1            ; simbolo seleccionado en el panel
estado          DB ESTADO_JUGANDO
semilla         DW 0            ; estado del generador pseudoaleatorio

; --- Sprite de cada ficha segun su ID (indice (ID-1)*2, son palabras) ---
; IDs 1..6 se usan en los tres niveles; 7 y 8 solo en Dificil.
tablaFichas     DW OFFSET MARIPOSAMORPHO_0, OFFSET MARIMBA_0    ; 1, 2
                DW OFFSET YIGUIRRO_0,       OFFSET VOLCAN_0     ; 3, 4
                DW OFFSET GUARIAMORADA_0,   OFFSET TORTUGA_0    ; 5, 6
                DW OFFSET CAFE_0,           OFFSET CARRETA_0    ; 7, 8
                

                
; --- Interfaz: menu, animacion y fuente de texto ---
opcionMenu      DB OPC_JUGAR    ; opcion resaltada en el menu principal
selNivel        DB 1            ; nivel resaltado en la pantalla de dificultad
pagInstruc      DB 0            ; pagina de instrucciones que se muestra

cuadroAnim      DB 0            ; cuadro actual de la mascota (0 o 1)
ticsUlt         DW 0            ; tics del BIOS cuando cambio el cuadro
animX           DW 144          ; esquina de la mascota animada
animY           DW 98

; Fuente de 8x8 que ya trae el BIOS (INT 10h AX=1130h). DibujarCaracter
; la usa para dibujar texto con pixeles propios a cualquier tamano.
fuenteBajaSeg   DW 0            ; caracteres 0..127
fuenteBajaOff   DW 0
fuenteAltaSeg   DW 0            ; caracteres 128..255 (acentos, enie)
fuenteAltaOff   DW 0
glifo           DB 8 DUP(0)     ; las 8 filas del caracter que se dibuja


; --- Fase 6: partida grafica ---
modoPrueba      DB 0            ; 1 = el secreto se escribe a mano (tecla T en el menu)

; Animacion generica: sprites de 32x32 y color con el que se borra su zona
animSpr0        DW OFFSET MASCOTA_0
animSpr1        DW OFFSET MASCOTA_1
animFondo       DB 16           ; indice de C_FONDO; PANEL lo cambia a C_PANEL

; Geometria del tablero por nivel (5 bytes por fila, igual que tablaNiveles):
;   filas por columna, alto de fila, ancho de columna, paso entre fichas,
;   paso entre iconos de retroalimentacion.
; Facil (12 intentos) usa 2 columnas de 6 filas para que TODO el historial
; quepa a la vez en los 168 px del tablero; Normal y Dificil, 1 columna de 10.
tablaGeom       DB  6, 26,  97, 16, 8   ; facil
                DB 10, 16, 196, 18, 9   ; normal
                DB 10, 16, 196, 18, 9   ; dificil
geomFilas       DB 10
geomAlto        DB 16
geomAncho       DB 196
geomPaso        DB 18
geomFb          DB 9
filaX           DW 0            ; esquina de la fila que se dibuja (ObtenerPosFila)
filaY           DW 0

numBuf          DB 4 DUP(0)     ; digitos de un numero convertido a texto
diez            DB 10           ; divisor para separar decenas y unidades

msgVis          DB 0            ; 1 = la barra de estado muestra un aviso (no la ayuda)

; --- Fase 7: fondo opcional para DibujarSprite (anti-parpadeo) ---
; 0FFFFh = los pixeles transparentes no se escriben (comportamiento normal).
; Un valor 0..255 = los pixeles transparentes se pintan de ese color, asi el
; sprite se dibuja en UNA sola pasada sin borrar antes (RG-08).
fondoSprite     DW 0FFFFh



; --- Extra: cronometro de partida (tics del BIOS) ---
; fondoTexto: igual que fondoSprite pero para texto. 0FFFFh = sin fondo;
; 0..255 = los pixeles apagados del glifo se pintan con ese color.
fondoTexto      DW 0FFFFh
ticsPartida     DW 0            ; tics del BIOS al empezar la partida (parte baja)
segMostrados    DW 0FFFFh       ; ultimo valor dibujado en el encabezado
tiempoFinal     DW 0            ; segundos que duro la ultima partida
tiempoBuf       DB 'Tiempo 00:00',0     ; los digitos empiezan en el byte 7
tiempoFinBuf    DB 'Tiempo: 00:00',0    ; los digitos empiezan en el byte 8

; --- Extra: sonido por el altavoz interno ---
; Cada nota: DW divisor (frecuencia = 1193180 / divisor), DW duracion en tics.
; El divisor 0 cierra la tabla.
sonPtr          DW 0            ; proxima nota por tocar (0 = silencio)
sonIni          DW 0            ; tics del BIOS cuando empezo la nota actual
sonDur          DW 0            ; duracion de la nota actual en tics

sndCambio       DW 1356, 1, 0                           ; 880 Hz: clic al mover o poner ficha
sndError        DW 5966, 3, 0                           ; 200 Hz: intento incompleto
sndConfirmar    DW 2386, 1, 1705, 1, 0                  ; 500 Hz y 700 Hz: intento aceptado
sndVictoria     DW 2281, 3, 1811, 3, 1522, 3, 1140, 6, 0   ; do-mi-sol-do agudo, sube
sndDerrota      DW 3044, 4, 3616, 4, 4554, 4, 6088, 8, 0   ; sol-mi-do-sol grave, baja

; --- Extra: fundido de paleta entre pantallas ---
brillo          DB 16           ; nivel actual de la paleta: 0 = negro, 16 = original
fundidoPend     DB 0            ; 1 = la pantalla recien dibujada aparece con fundido