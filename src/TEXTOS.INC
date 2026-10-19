;=====================================================================
; TEXTOS.INC - Cadenas de las pantallas (terminadas en 0)
; La fuente del BIOS es CP437: los acentos se escriben como numeros
; para que no dependan de la codificacion con que se guarde el archivo.
;   a = 0A0h   e = 82h   i = 0A1h   o = 0A2h   u = 0A3h   n = 0A4h
;=====================================================================

; ---- Presentacion ----
tHdrPres    DB 'EIF205 - Proyecto II',0
tTitulo     DB 'MASTERMIND',0
tSubtitulo  DB 'S',0A1h,'mbolos de Costa Rica',0
tIntegr     DB 'Integrantes:',0
tAutor1     DB 'Mathew Ramirez',0
tAutor2     DB 'Alen Cedeno',0
tCurso1     DB 'EIF205 Arquitectura de Computadores',0
tCurso2     DB 'UNA Sede Brunca - II Ciclo 2026',0
tBarraPres  DB 'Presiona una tecla para continuar',0

; ---- Menu ----
tHdrMenu    DB 'MENU PRINCIPAL',0
tBarraMenu  DB 24,' ',25,' mover     ENTER elegir',0
tOpJugar    DB 'Jugar',0
tOpNivel    DB 'Dificultad',0
tOpInstruc  DB 'Instrucciones',0
tOpCredit   DB 'Cr',82h,'ditos',0
tOpSalir    DB 'Salir',0
opcionesMenu DW OFFSET tOpJugar, OFFSET tOpNivel, OFFSET tOpInstruc
             DW OFFSET tOpCredit, OFFSET tOpSalir
tFlechaIzq  DB 16,0             ; triangulo hacia la derecha (CP437)
tFlechaDer  DB 17,0             ; triangulo hacia la izquierda
tNivelAct   DB 'Nivel actual:',0

; ---- Niveles ----
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

; ---- Instrucciones ----
tHdrInstr   DB 'INSTRUCCIONES',0
tBarraI1    DB 'ENTER o ',17,16,' siguiente   ESC menu',0
tBarraI2    DB 17,' anterior   ESC menu',0
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
tNiveles    DB 'NIVELES',0
tNivF1      DB 'F',0A0h,'cil: 4 posiciones, 6 s',0A1h,'mbolos,',0
tNivF2      DB '  sin repetir, 12 intentos.',0
tNivN1      DB 'Normal: 4 posiciones, 6 s',0A1h,'mbolos,',0
tNivN2      DB '  con repetici',0A2h,'n, 10 intentos.',0
tNivD1      DB 'Dif',0A1h,'cil: 5 posiciones, 8 s',0A1h,'mbolos,',0
tNivD2      DB '  con repetici',0A2h,'n, 10 intentos.',0
tControles  DB 'CONTROLES',0
tCtl1       DB 17,' ',16,'  cambiar de posici',0A2h,'n',0
tCtl2       DB 24,' ',25,' o 1-8  elegir s',0A1h,'mbolo',0
tCtl3       DB 'ENTER confirmar  BACKSPACE borrar',0
tCtl4       DB 'ESC volver al men',0A3h,0

; ---- Creditos ----
tHdrCred    DB 'CR',90h,'DITOS',0
tCred1      DB 'Proyecto II - S',0A1h,'mbolos de Costa Rica',0
tCred3      DB 'Integrantes',0
tCred4      DB 'Universidad Nacional de Costa Rica',0
tCred5      DB 'Sede Regional Brunca',0
tCred6      DB 'EIF205 Arquitectura de Computadores',0
tCred7      DB 'II Ciclo 2026',0
tCred8      DB 'Arte original hecho en Aseprite',0
tBarraVolver DB 'Presiona una tecla para volver',0

; ---- Partida (provisional hasta la Fase 6) ----
tHdrJuego   DB 'PARTIDA',0
tJuego1     DB 'El tablero se integra en la Fase 6.',0
tJuego2     DB 'Por ahora esta pantalla confirma',0
tJuego3     DB 'el nivel elegido en el men',0A3h,'.',0
tBarraJuego DB 'Presiona una tecla para volver al men',0A3h,0




; ---- Partida grafica ----
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

; ---- Modo prueba (secreto manual) ----
tModoPrueba DB '[MODO PRUEBA]',0
tHdrPrueba  DB 'MODO PRUEBA',0
tPrueba1    DB 'Escribe el secreto con las teclas',0
tPrueba2    DB 'de los s',0A1h,'mbolos (1, 2, 3...)',0
tBarraPrueba DB 'ESC usa un secreto aleatorio',0

; ---- Fin de partida ----
tHdrFin     DB 'FIN DE LA PARTIDA',0
tVictoria   DB 'VICTORIA',0
tDerrota    DB 'DERROTA',0
tUsados     DB 'Intentos usados:',0
tSecreto    DB 'El secreto era:',0
tBarraFin   DB 'ENTER jugar de nuevo   ESC men',0A3h,0