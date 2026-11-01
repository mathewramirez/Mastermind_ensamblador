
; TEXTOS.INC - Cadenas de las pantallas (terminadas en 0)
; La fuente del BIOS es CP437: los acentos se escriben como numeros
; para que no dependan de la codificacion con que se guarde el archivo.
;   a = 0A0h   e = 82h   i = 0A1h   o = 0A2h   u = 0A3h   n = 0A4h

;Presentacion 
tHdrPres    DB 'EIF205 - Proyecto II',0
tTitulo     DB 'MASTERMIND',0
tSubtitulo  DB 'S',0A1h,'mbolos de Costa Rica',0
tIntegr     DB 'Integrantes:',0
tAutor1     DB 'Mathew Ramirez',0
tAutor2     DB 'Alen Cedeno',0
tCurso1     DB 'EIF205 Arquitectura de Computadores',0
tCurso2     DB 'UNA Sede Brunca - II Ciclo 2026',0
tBarraPres  DB 'Presiona una tecla para continuar',0

;Menu
tHdrMenu    DB 'MENU PRINCIPAL',0
tBarraMenu  DB 24,' ',25,' mover     ENTER elegir',0
tOpJugar    DB 'Jugar',0
tOpNivel    DB 'Dificultad',0
tOpJug1     DB 'Jugadores: 1',0
tOpJug2     DB 'Jugadores: 2',0
tOpInverso  DB 'Modo inverso',0
tOpPuntajes DB 'Mejores puntajes',0
tOpInstruc  DB 'Instrucciones',0
tOpCredit   DB 'Cr',82h,'ditos',0
tOpSalir    DB 'Salir',0
; el orden debe coincidir con OPC_xxx de CONST.INC
; (la opcion Jugadores cambia entre tOpJug1 y tOpJug2 en DibujarOpcion)
opcionesMenu DW OFFSET tOpJugar, OFFSET tOpNivel, OFFSET tOpJug1, OFFSET tOpInverso
             DW OFFSET tOpPuntajes, OFFSET tOpInstruc, OFFSET tOpCredit, OFFSET tOpSalir
tFlechaIzq  DB 16,0             ; triangulo hacia la derecha (CP437)
tFlechaDer  DB 17,0             ; triangulo hacia la izquierda
tNivelAct   DB 'Nivel actual:',0

;Niveles
tFacil      DB 'F',0A0h,'cil',0
tNormal     DB 'Normal',0
tDificil    DB 'Dif',0A1h,'cil',0
nombresNivel DW OFFSET tFacil, OFFSET tNormal, OFFSET tDificil
tHdrNivel   DB 'NIVEL DE DIFICULTAD',0
tBarraNivel DB 24,' ',25,' mover   ENTER elegir   ESC volver',0
tDet1F      DB '4 posiciones, 6 s',0A1h,'mbolos',0
tDet2F      DB 'Sin repetir - 12 intentos',0
tDet1N      DB '4 posiciones, 6 s',0A1h,'mbolos',0
tDet2N      DB 'Con repetici',0A2h,'n - 10 intentos',0
tDet1D      DB '5 posiciones, 8 s',0A1h,'mbolos',0
tDet2D      DB 'Con repetici',0A2h,'n - 10 intentos',0
detalle1    DW OFFSET tDet1F, OFFSET tDet1N, OFFSET tDet1D
detalle2    DW OFFSET tDet2F, OFFSET tDet2N, OFFSET tDet2D

;Instrucciones
tHdrInstr   DB 'INSTRUCCIONES',0
tBarraI1    DB 'ENTER o ',17,16,' siguiente   ESC menu',0
tBarraI2    DB 17,' anterior   ESC menu',0
tBarraI4    DB 17,16,' p',0A0h,'ginas   ESC men',0A3h,0
tObjetivo   DB 'OBJETIVO',0
tObj1       DB 'Adivina el c',0A2h,'digo secreto de',0
tObj2       DB 's',0A1h,'mbolos antes de quedarte sin',0
tObj3       DB 'intentos. Cada intento da pistas:',0
tExacto1    DB 'EXACTO (toro): s',0A1h,'mbolo correcto',0
tExacto2    DB 'en la posici',0A2h,'n correcta.',0
tParcial1   DB 'PARCIAL (vaca): s',0A1h,'mbolo correcto',0
tParcial2   DB 'pero en otra posici',0A2h,'n.',0
tNinguna    DB 'Sin iconos: ese s',0A1h,'mbolo no est',0A0h,'.',0
tEjemplo    DB 'Un s',0A1h,'mbolo del secreto solo cuenta',0
tEjemplo2   DB 'una vez: no se repite la pista.',0
; Pagina 2: niveles (3 lineas de detalle por nivel, 34 caracteres como maximo)
tNivF0      DB 'F',0A0h,'cil - 12 intentos',0
tNivF1      DB '4 posiciones, 6 s',0A1h,'mbolos (1-6).',0
tNivF2      DB 'El secreto no repite s',0A1h,'mbolos.',0
tNivF3      DB 'Ideal para aprender a jugar.',0
tNivN0      DB 'Normal - 10 intentos',0
tNivN1      DB '4 posiciones, 6 s',0A1h,'mbolos (1-6).',0
tNivN2      DB 'El secreto puede repetir s',0A1h,'mbolos;',0
tNivN3      DB 'usa las pistas para descartarlos.',0
tNivD0      DB 'Dif',0A1h,'cil - 10 intentos',0
tNivD1      DB '5 posiciones, 8 s',0A1h,'mbolos (1-8).',0
tNivD2      DB 'Con repetici',0A2h,'n: es el mayor reto.',0
tNivD3      DB 'Deduce con cuidado cada intento.',0
; Pagina 3: controles
tControles  DB 'CONTROLES',0
tCtl1       DB 17,' ',16,'  cambiar de posici',0A2h,'n',0
tCtl2       DB 24,' ',25,'  cambiar el s',0A1h,'mbolo',0
tCtl2b      DB '1-6 (F/N) o 1-8 (D): elegir directo',0
tCtl3       DB 'ENTER  confirmar el intento',0
tCtl3b      DB 'Si falta alguno, no gasta intento.',0
tCtl5       DB 'BACKSPACE  borrar la posici',0A2h,'n',0
tCtl4       DB 'ESC  salir (pide confirmaci',0A2h,'n)',0
; Pagina 4: dos jugadores
tDosH       DB 'DOS JUGADORES',0
tDos1       DB 'En el men',0A3h,' elige Jugadores: 2',0
tDos2       DB 'con ENTER o ',17,16,'.',0
tDos3       DB 'Son 2 rondas con el mismo nivel:',0
tDos4       DB 'uno escribe el secreto sin que el',0
tDos5       DB 'otro mire, y el otro intenta',0
tDos6       DB 'adivinarlo. En la ronda 2 se',0
tDos7       DB 'cambian los papeles.',0
tDosG       DB 'Qui',82h,'n gana',0
tDos8       DB 'Quien adivine con menos intentos.',0
tDos9       DB 'Si empatan, el m',0A0h,'s r',0A0h,'pido.',0
tDos10      DB 'No adivinar es el peor resultado.',0
tDos11      DB 'El ganador va a Mejores puntajes.',0
; Pagina 5: modo inverso (la computadora adivina)
tInvH       DB 'MODO INVERSO',0
tInv1       DB 'En el men',0A3h,' elige Modo inverso.',0
tInv2       DB 'Piensa un c',0A2h,'digo (no lo digas).',0
tInv3       DB 'La computadora lo deduce: propone',0
tInv4       DB 'un intento y t',0A3h,' le respondes',0
tInv5       DB 'cu',0A0h,'ntos hay de cada tipo:',0
tInv6       DB 24,25,' cambia el n',0A3h,'mero; ',17,16,' el campo.',0
tInv7       DB 'ENTER confirma tu respuesta.',0
tInv8       DB 'Una respuesta imposible se rechaza.',0

;Creditos
tHdrCred    DB 'CR',90h,'DITOS',0
tCred1      DB 'Proyecto II - S',0A1h,'mbolos de Costa Rica',0
tCred3      DB 'Integrantes',0
tCred4      DB 'Universidad Nacional de Costa Rica',0
tCred5      DB 'Sede Regional Brunca',0
tCred6      DB 'EIF205 Arquitectura de Computadores',0
tCred7      DB 'II Ciclo 2026',0
tCred8      DB 'Arte original hecho en Aseprite',0
tBarraVolver DB 'Presiona una tecla para volver',0

;Partida (provisional hasta la Fase 6)
tHdrJuego   DB 'PARTIDA',0
tJuego1     DB 'El tablero se integra en la Fase 6.',0
tJuego2     DB 'Por ahora esta pantalla confirma',0
tJuego3     DB 'el nivel elegido en el men',0A3h,'.',0
tBarraJuego DB 'Presiona una tecla para volver al men',0A3h,0

;Partida grafica
tVacio      DB 0
tAyudaJ     DB 27,26,24,25,' mover ENTER ok BS borra ESC menu',0
tIncomp     DB 'Intento incompleto: llena todo',0
tFinJ       DB 'Partida terminada - pulsa una tecla',0
tNivelH     DB 'Nivel: ',0
tPaletaL    DB 'FICHAS',0
tIntentoL   DB 'TU INTENTO',0
tIntento    DB 'Intento',0
tQuedan     DB 'Quedan',0
tBarraNum   DB '/',0

;Modo prueba (secreto manual)
tModoPrueba DB '[MODO PRUEBA]',0
tHdrPrueba  DB 'MODO PRUEBA',0
tPrueba1    DB 'Escribe el secreto con las teclas',0
tPrueba2    DB 'de los s',0A1h,'mbolos (1, 2, 3...)',0
tBarraPrueba DB 'ESC usa un secreto aleatorio',0

;Fin de partida
tHdrFin     DB 'FIN DE LA PARTIDA',0
tVictoria   DB 'VICTORIA',0
tDerrota    DB 'DERROTA',0
tUsados     DB 'Intentos usados:',0
tSecreto    DB 'El secreto era:',0
tBarraFin   DB 'ENTER jugar de nuevo   ESC men',0A3h,0

;Aviso del secreto manual en nivel Facil 
tRepetido   DB 'F',0A0h,'cil: no se permiten repetidos',0

; Mejores puntajes 
tHdrPunt    DB 'MEJORES PUNTAJES',0
tBarraPunt  DB 24,25,' tipo  ',17,16,' nivel  ESC menu',0
; titulos de nivel de 12 caracteres fijos: al cambiar de nivel se redibujan
; encima uno del otro sin borrar (anti-parpadeo)
tPuntN0     DB '   F',0A0h,'cil    ',0
tPuntN1     DB '   Normal   ',0
tPuntN2     DB '  Dif',0A1h,'cil   ',0
tituloPunt  DW OFFSET tPuntN0, OFFSET tPuntN1, OFFSET tPuntN2
tPuntIz     DB 17,0
tPuntDe     DB 16,0
tColNum     DB '#',0
tColNombre  DB 'Nombre',0
tColInt     DB 'Intentos',0
tColTiempo  DB 'Tiempo',0
tPuntVNom   DB '---',0
tPuntVInt   DB '--',0
tPuntVTiempo DB '--:--',0
tPuntNota   DB 'Menos intentos y luego menos tiempo',0
tPuntErr    DB 'No se pudo guardar el archivo',0

;Nuevo record (iniciales)
tHdrNuevoRec DB 'NUEVO R',90h,'CORD',0
tBarraNombre DB 'A-Z 0-9  BS borra  ENTER ok  ESC omite',0
tRecTit     DB 'Entraste al top 5',0
tPuesto     DB 'Puesto',0
tRecIntro   DB 'Escribe tus iniciales:',0
tRecordFin  DB 'Nuevo r',82h,'cord: puesto ',0

;Dos jugadores
; Las cadenas con un digito (JUGADOR 1, GANA EL JUGADOR 1...) se modifican en
; memoria antes de dibujarlas: el digito esta en la posicion que indica el comentario.
tHdrSecJ    DB 'JUGADOR 1: SECRETO',0      ; digito en el byte 8
tSecretoJ   DB 'El otro jugador no debe mirar',0
tJugadorL   DB 'JUGADOR 2',0               ; digito en el byte 8
tHdrResDos  DB 'RESULTADO DE 2 JUGADORES',0
tGanaJ      DB 'GANA EL JUGADOR 1',0       ; digito en el byte 16
tEmpate     DB 'EMPATE',0
tResJ       DB 'Jugador 1:',0              ; digito en el byte 8
tResInt     DB 'intento(s)',0
tResNo      DB 'no adivin',0A2h,0
tTurnoJ     DB 'TURNO: J1',0           ; digito en el byte 8 (encabezado, modo 2 jugadores)

;Modo inverso
tHdrInv     DB 'MODO INVERSO',0
tHdrFinInv  DB 'FIN DEL MODO INVERSO',0
tInvTu      DB 'TU RESPUESTA',0
tInvMi      DB 'MI INTENTO',0
tToros      DB 'Exacto',0
tVacas      DB 'Parcial',0
tInvAyuda   DB 24,25,' cambia ',27,26,' campo ENTER ok ESC menu',0
tInvPensando DB 'Pensando...',0
tInvImposible DB 'Respuesta imposible: revisa los numeros',0
tInvSinSol  DB 'No hay codigo posible: revisa respuestas',0
tInvGano    DB 'LO ADIVIN',90h,0
tInvPerdio  DB 'ME GANASTE',0
tInvFalla   DB 'No acert',82h,' en mis intentos',0

;Confirmacion al salir de una partida
tSalirT1    DB 'Salir de la partida?',0
tSalirT2    DB 'Perder',0A0h,'s el avance',0
tSalirT3    DB 'S salir    N seguir',0

;Mejores puntajes: categorias (14 caracteres c/u, para tapar la anterior)
tCat0       DB '  1 JUGADOR   ',0
tCat1       DB ' 2 JUGADORES  ',0
tCat2       DB ' COMPUTADORA  ',0
catPunt     DW OFFSET tCat0, OFFSET tCat1, OFFSET tCat2
tPuntArrA   DB 24,0             ; flecha arriba (CP437)
tPuntArrB   DB 25,0             ; flecha abajo
tRecIntro2  DB 'Jugador 1: escribe tus iniciales',0   ; el digito esta en el byte 8
