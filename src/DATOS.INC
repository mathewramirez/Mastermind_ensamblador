; DATOS.INC - Variables y tablas del juego (van dentro de .DATA)

modoOriginal    DB ?            

; Configuracion del nivel activo 
nivel           DB NIVEL_NORMAL 
numPos          DB 4            
numColores      DB 6           
repeticion      DB 1            ; 1 = el secreto puede repetir simbolos
maxIntentos     DB 10

; Una fila de 4 bytes por nivel: N, C, repeticion, intentos (Tabla 1)

tablaNiveles    DB 4, 6, 0, 12  ; facil
                DB 4, 6, 1, 10  ; normal
                DB 5, 8, 1, 10  ; dificil

;  Estado de la partida
secreto         DB MAX_N DUP(0) 
intento         DB MAX_N DUP(0) 
copiaS          DB MAX_N DUP(0) 
copiaI          DB MAX_N DUP(0) 

; Historial por filas: el simbolo j del intento t esta en historial[t*MAX_N + j]
historial       DB (MAX_INTENTOS*MAX_N) DUP(0)
histExactos     DB MAX_INTENTOS DUP(0)  
histParciales   DB MAX_INTENTOS DUP(0)  

turno           DB 0           
posActiva       DB 0            
colorSel        DB 1            
estado          DB ESTADO_JUGANDO
semilla         DW 0            ; estado del generador pseudoaleatorio

; IDs 1..6 se usan en los tres niveles; 7 y 8 solo en Dificil.
tablaFichas     DW OFFSET MARIPOSAMORPHO_0, OFFSET MARIMBA_0    
                DW OFFSET YIGUIRRO_0,       OFFSET VOLCAN_0     
                DW OFFSET GUARIAMORADA_0,   OFFSET TORTUGA_0    
                DW OFFSET CAFE_0,           OFFSET CARRETA_0    

; Interfaz: menu, animacion y fuente de texto 
opcionMenu      DB OPC_JUGAR   
selNivel        DB 1           
pagInstruc      DB 0            

cuadroAnim      DB 0            
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

;  partida grafica
modoPrueba      DB 0            

; Animacion generica: sprites de 32x32 y color con el que se borra su zona
animSpr0        DW OFFSET MASCOTA_0
animSpr1        DW OFFSET MASCOTA_1
animFondo       DB 16           ; indice de C_FONDO; PANEL lo cambia a C_PANEL

; Geometria del tablero por nivel (5 bytes por fila, igual que tablaNiveles):

; Geometria del tablero por nivel (5 bytes por fila, igual que tablaNiveles):

tablaGeom       DB  6, 26,  97, 16, 8   ; facil
                DB 10, 16, 196, 24, 18  ; normal
                DB 10, 16, 196, 18, 16  ; dificil
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

;  fondo opcional para DibujarSprite (anti-parpadeo) 

fondoSprite     DW 0FFFFh

;Extra: cronometro de partida (tics del BIOS)

fondoTexto      DW 0FFFFh
ticsPartida     DW 0            ; tics del BIOS al empezar la partida (parte baja)
segMostrados    DW 0FFFFh       
tiempoFinal     DW 0            
tiempoBuf       DB 'Tiempo 00:00',0   
tiempoFinBuf    DB 'Tiempo: 00:00',0   

; Extra: sonido por el altavoz interno
; Cada nota: DW divisor (frecuencia = 1193180 / divisor), DW duracion en tics.

sonPtr          DW 0            ; proxima nota por tocar (0 = silencio)
sonIni          DW 0            ; tics del BIOS cuando empezo la nota actual
sonDur          DW 0            ; duracion de la nota actual en tics

sndCambio       DW 1356, 1, 0                           
sndError        DW 5966, 3, 0                          
sndConfirmar    DW 2386, 1, 1705, 1, 0                  
sndVictoria     DW 2281, 3, 1811, 3, 1522, 3, 1140, 6, 0  
sndDerrota      DW 3044, 4, 3616, 4, 4554, 4, 6088, 8, 0  

;  Extra: fundido de paleta entre pantallas
brillo          DB 16           ; nivel actual de la paleta: 0 = negro, 16 = original
fundidoPend     DB 0            ; 1 = la pantalla recien dibujada aparece con fundido

; Casilla del intento armada en memoria  , estos son 2 buffer pequeños de 400 y 41
casBuf          DB CASILLA_LADO * CASILLA_LADO DUP(0)

; Barra de estado armada en memoria 
estBuf          DB 41 DUP(0)    ; 40 columnas + el 0 final

; Extra: mejores puntajes persistidos en archivo

puntArch        DB 'M','3'             
puntTabla       DB PUNT_TOTAL DUP(0)    ; 3 categorias x 3 niveles x 5 registros x 6 bytes
puntNombreArch  DB 'MEJORES.DAT',0      ; se crea en la carpeta desde donde se ejecuta
puntError       DB 0           
puntPuesto      DB 0FFh         
selPunt         DB 0            
puntCat         DB 0           
puntFila        DB 0           
puntY           DW 0            
nombreTmp       DB ' ',' ',' ',0        ; iniciales que escribe el jugador
nombreLen       DB 0            
chBuf           DB 0,0         
nomBuf          DB ' ',' ',' ',0        
tiempoPunt      DB '00:00',0   

; Extra: modo dos jugadores por turnos 

modo2j          DB 0            ; 1 = modo dos jugadores (se alterna en el menu)
jugCreador      DB 1            
dosInt          DB 2 DUP(0)     
dosTiempo          DW 2 DUP(0)     

;  Extra: modo inverso (el programa deduce el codigo del jugador) 
invCand         DB MAX_N DUP(0)       
invCnt          DB 9 DUP(0)            
histCnt         DB MAX_INTENTOS*9 DUP(0) 
invResp         DB 2 DUP(0)            
invCampo        DB 0                   ; campo activo: 0 = toros, 1 = vacas
invTmp          DB 0                   ; toros del candidato frente al intento que se compara
; Primer intento del programa por nivel (MAX_N bytes por nivel): reparte los
; simbolos en 2 grupos, lo que da mas informacion que 1,1,1,1.
invApertura     DB 1,2,3,4,0           ; facil (sin repetidos)
                DB 1,1,2,2,0           ; normal
                DB 1,1,2,2,3           ; dificil
