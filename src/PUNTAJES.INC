
; PUNTAJES.INC - Mejores puntajes persistidos en un archivo (opcional)


; LimpiarPuntajes
;   Recibe:    nada
;   Devuelve:  firma 'M3' y todas las tablas vacias (intentos = 0)
;   Destruye:  nada
LimpiarPuntajes PROC
        push ax
        push cx
        push di
        push es

        push ds
        pop  es                 ; ES = DS para STOSB
        cld
        mov  puntArch, 'M'
        mov  puntArch + 1, '3'
        xor  al, al
        LIMPIAR_ARREGLO puntTabla, PUNT_TOTAL

        pop  es
        pop  di
        pop  cx
        pop  ax
        ret
LimpiarPuntajes ENDP

; CargarPuntajes
;   Recibe:    nada
;   Devuelve:  puntTabla con lo guardado en MEJORES.DAT; si el archivo no
;              existe, esta incompleto o no lleva la firma, tablas vacias
;   Destruye:  nada
CargarPuntajes PROC
        push ax
        push bx
        push cx
        push dx

        call LimpiarPuntajes    ; valores por defecto si algo falla
        mov  ax, 3D00h          ; AH=3Dh abrir, AL=0 solo lectura
        mov  dx, OFFSET puntNombreArch
        int  21h
        jc   cpFin              ; no existe (primera vez): tablas vacias
        mov  bx, ax             ; BX = handle del archivo

        mov  ah, 3Fh            ; leer el archivo completo sobre puntArch
        mov  cx, PUNT_ARCH
        mov  dx, OFFSET puntArch
        int  21h
        mov  cx, ax             ; CX = bytes leidos (MOV no cambia CF)
        pushf                   ; recordar si la lectura fallo
        mov  ah, 3Eh            ; cerrar siempre el archivo
        int  21h
        popf
        jc   cpMal
        cmp  puntArch, 'M'
        jne  cpMal
        cmp  cx, PUNT_ARCH_V1
        jne  cpNuevo
        ; Archivo de la version anterior (92 bytes, firma 'MC'): sus tablas son
        ; justo las de la categoria 1 jugador, asi que se conservan tal cual.
        cmp  puntArch + 1, 'C'
        jne  cpMal
        mov  puntArch + 1, '3'  ; pasa a la firma nueva; las demas categorias quedan vacias
        jmp  cpFin
cpNuevo:
        cmp  cx, PUNT_ARCH
        jne  cpMal              ; archivo incompleto
        cmp  puntArch + 1, '3'
        je   cpFin              ; firma correcta: datos validos
cpMal:
        call LimpiarPuntajes    ; la lectura pudo dejar basura: empezar de cero
cpFin:
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
CargarPuntajes ENDP

; GuardarPuntajes
;   Recibe:    puntArch / puntTabla
;   Devuelve:  MEJORES.DAT reescrito; puntError = 1 si algo fallo
;   Destruye:  nada
GuardarPuntajes PROC
        push ax
        push bx
        push cx
        push dx

        mov  puntError, 0
        mov  ah, 3Ch            ; crear (o vaciar) el archivo
        xor  cx, cx             ; atributos normales
        mov  dx, OFFSET puntNombreArch
        int  21h
        jc   gpError
        mov  bx, ax             ; BX = handle

        mov  ah, 40h            ; escribir firma + tablas
        mov  cx, PUNT_ARCH
        mov  dx, OFFSET puntArch
        int  21h
        mov  dx, ax             ; DX = bytes escritos
        pushf
        mov  ah, 3Eh            ; cerrar siempre
        int  21h
        popf
        jc   gpError
        cmp  dx, PUNT_ARCH
        je   gpFin              ; se escribio todo
gpError:
        mov  puntError, 1
gpFin:
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
GuardarPuntajes ENDP

; InicioTabla
;   Recibe:    AL = nivel (0..2), puntCat (categoria en uso)
;   Devuelve:  AX = desplazamiento en puntTabla de la tabla de esa
;              categoria y nivel: puntCat * 90 + nivel * 30
;   Destruye:  AX
InicioTabla PROC
        push bx
        mov  bl, PUNT_NIVEL
        mul  bl                 ; AX = nivel * 30
        mov  bx, ax
        mov  al, puntCat
        mov  ah, PUNT_CAT
        mul  ah                 ; AX = categoria * 90
        add  ax, bx
        pop  bx
        ret
InicioTabla ENDP

; BuscarPuesto
;   Recibe:    nivel, puntCat, turno (intentos usados), tiempoFinal (segundos)
;   Devuelve:  AL = puesto 0..4 que le toca a esta partida en la tabla
;              de su categoria y nivel, o 0FFh si no entra al top 5
;   Destruye:  AL (es el valor de retorno)
;   Un puesto vacio siempre sirve; si no, se compara con el registro que
;   ya esta: menos intentos gana y, a igual numero, menos segundos.
BuscarPuesto PROC
        push bx
        push cx
        push dx
        push si

        mov  al, nivel
        call InicioTabla
        mov  si, ax             ; SI = inicio de la tabla de esta categoria y nivel
        xor  cl, cl             ; CL = puesto que se prueba
bpSiguiente:
        cmp  cl, PUNT_TOP
        jae  bpNoEntra
        mov  al, cl
        mov  bl, PUNT_REC
        mul  bl
        mov  bx, ax
        add  bx, si             ; BX = registro de ese puesto
        mov  al, puntTabla[bx+3]
        or   al, al
        jz   bpEntra            ; registro vacio: ocupa el puesto
        cmp  turno, al
        jb   bpEntra            ; menos intentos que el registro
        ja   bpProbarOtro
        mov  dx, tiempoFinal
        cmp  dx, WORD PTR puntTabla[bx+4]
        jb   bpEntra            ; mismos intentos y menos tiempo
bpProbarOtro:
        inc  cl
        jmp  bpSiguiente
bpEntra:
        mov  al, cl
        jmp  bpFin
bpNoEntra:
        mov  al, 0FFh
bpFin:
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
BuscarPuesto ENDP

; InsertarPuntaje
;   Recibe:    AL = puesto 0..4, nombreTmp (iniciales), nivel, puntCat,
;              turno, tiempoFinal
;   Devuelve:  el registro nuevo en ese puesto; los de abajo bajan un
;              lugar y el ultimo sale de la tabla
;   Destruye:  nada
InsertarPuntaje PROC
        push ax
        push bx
        push cx
        push dx
        push si

        mov  dl, al             ; DL = puesto
        mov  al, nivel
        call InicioTabla
        mov  si, ax             ; SI = inicio de la tabla de la categoria y el nivel

        ; Hacer lugar: copiar los registros del penultimo hasta el puesto
        ; un lugar hacia abajo, empezando por el final para no pisarlos.
        mov  cl, PUNT_TOP - 2
ipBajar:
        cmp  cl, dl
        jl   ipEscribir         ; comparacion con signo: CL llega a -1
        mov  al, cl
        mov  bl, PUNT_REC
        mul  bl
        add  ax, si
        mov  bx, ax             ; BX = registro CL
        mov  ax, WORD PTR puntTabla[bx]
        mov  WORD PTR puntTabla[bx + PUNT_REC], ax
        mov  ax, WORD PTR puntTabla[bx + 2]
        mov  WORD PTR puntTabla[bx + PUNT_REC + 2], ax
        mov  ax, WORD PTR puntTabla[bx + 4]
        mov  WORD PTR puntTabla[bx + PUNT_REC + 4], ax
        dec  cl
        jmp  ipBajar

ipEscribir:
        mov  al, dl
        mov  bl, PUNT_REC
        mul  bl
        add  ax, si
        mov  bx, ax             ; BX = registro del puesto nuevo
        mov  al, nombreTmp
        mov  puntTabla[bx], al
        mov  al, nombreTmp + 1
        mov  puntTabla[bx + 1], al
        mov  al, nombreTmp + 2
        mov  puntTabla[bx + 2], al
        mov  al, turno
        mov  puntTabla[bx + 3], al
        mov  ax, tiempoFinal
        mov  WORD PTR puntTabla[bx + 4], ax

        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
InsertarPuntaje ENDP

; DibujarNombre
;   Recibe:    nombreTmp, nombreLen
;   Devuelve:  las 3 casillas de iniciales con su letra y, bajo la que
;              se escribe ahora, una marca ^. Cada letra se dibuja con
;              fondo propio, asi no hay un instante en blanco.
;   Destruye:  nada
DibujarNombre PROC
        push ax
        push bx
        push dx
        push si
        push di

        xor  si, si             ; SI = casilla 0..2
dnSlot:
        mov  ax, C_FILA
        mov  fondoTexto, ax     ; la letra pinta su casilla de 20x20
        mov  al, nombreTmp[si]
        mov  chBuf, al
        mov  ax, si
        mov  bl, 24             ; 20 px de casilla + 4 de separacion
        mul  bl
        add  ax, NOM_X
        mov  di, ax             ; DI = x de la casilla
        add  ax, 2              ; la letra de 16x16 va centrada
        mov  bx, NOM_Y + 2
        mov  dl, C_TEXTO
        mov  dh, 2
        push si
        mov  si, OFFSET chBuf
        call DibujarTexto
        pop  si

        mov  ax, C_FONDO
        mov  fondoTexto, ax     ; la marca se dibuja sobre el fondo de la pantalla
        mov  chBuf, ' '
        mov  ax, si
        cmp  al, nombreLen
        jne  dnSinMarca
        mov  chBuf, '^'         ; esta es la casilla activa
dnSinMarca:
        mov  ax, di
        add  ax, 6              ; caracter de 8 px centrado bajo la casilla
        mov  bx, NOM_Y + 24
        mov  dl, C_DORADO
        mov  dh, 1
        push si
        mov  si, OFFSET chBuf
        call DibujarTexto
        pop  si

        inc  si
        cmp  si, 3
        jb   dnSlot
        mov  fondoTexto, 0FFFFh

        pop  di
        pop  si
        pop  dx
        pop  bx
        pop  ax
        ret
DibujarNombre ENDP

; PedirNombre
;   Recibe:    puntPuesto (puesto logrado), nivel, puntCat (con 2 jugadores
;              el texto lleva el numero del ganador: tRecIntro2)
;   Devuelve:  AL = 1 si el jugador confirmo con ENTER (nombreTmp tiene sus
;              iniciales); AL = 0 si omitio con ESC
;   Destruye:  AX
;   Teclas: letras (se pasan a mayuscula) y digitos, BACKSPACE borra,
;   ENTER confirma (hace falta al menos 1 caracter), ESC omite. Las
;   demas teclas, incluidas las flechas (AL = 0), se ignoran.
PedirNombre PROC
        push bx
        push cx
        push dx
        push si
        push di

        mov  si, OFFSET tHdrNuevoRec
        mov  di, OFFSET tBarraNombre
        call PintarPantalla
        mov  nombreLen, 0
        mov  nombreTmp, ' '
        mov  nombreTmp + 1, ' '
        mov  nombreTmp + 2, ' '

        CENTRAR tRecTit, 34, C_DORADO, 2
        TEXTO tPuesto, 128, 62, C_TEXTO, 1
        mov  al, puntPuesto
        inc  al                 ; se muestra desde 1
        mov  cx, 184
        mov  bx, 62
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarNumero
        TEXTO tNivelH, 100, 74, C_TEXTO, 1
        mov  bl, nivel
        xor  bh, bh
        shl  bx, 1
        mov  si, nombresNivel[bx]
        mov  ax, 156
        mov  bx, 74
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarTexto
        cmp  puntCat, 1
        je   pnIntroDos
        CENTRAR tRecIntro, 96, C_TEXTO, 1
        jmp  pnCasillas
pnIntroDos:
        CENTRAR tRecIntro2, 96, C_TEXTO, 1
pnCasillas:
        RELLENAR_RECT C_FILA, NOM_X, NOM_Y, 20, 20
        RELLENAR_RECT C_FILA, NOM_X+24, NOM_Y, 20, 20
        RELLENAR_RECT C_FILA, NOM_X+48, NOM_Y, 20, 20
        call DibujarNombre
        call VaciarTeclado
pnLeer:
        call EsperarTecla       ; AH = scan code, AL = ASCII (0 en flechas)
        cmp  al, ASC_ESC
        je   pnOmitir
        cmp  al, ASC_ENTER
        je   pnConfirmar
        cmp  al, ASC_RETROCESO
        je   pnBorrar
        cmp  al, 'a'
        jb   pnClasificar
        cmp  al, 'z'
        ja   pnClasificar
        sub  al, 20h            ; minuscula -> mayuscula
pnClasificar:
        cmp  al, 'A'
        jb   pnDigito
        cmp  al, 'Z'
        jbe  pnPoner
pnDigito:
        cmp  al, '0'
        jb   pnLeer             ; cualquier otra tecla: ignorar
        cmp  al, '9'
        ja   pnLeer
pnPoner:
        cmp  nombreLen, 3
        jae  pnLeer             ; ya estan las 3 iniciales
        mov  bl, nombreLen
        xor  bh, bh
        mov  nombreTmp[bx], al
        inc  nombreLen
        call DibujarNombre
        jmp  pnLeer
pnBorrar:
        cmp  nombreLen, 0
        je   pnLeer
        dec  nombreLen
        mov  bl, nombreLen
        xor  bh, bh
        mov  nombreTmp[bx], ' '
        call DibujarNombre
        jmp  pnLeer
pnConfirmar:
        cmp  nombreLen, 0
        je   pnLeer             ; sin iniciales no se acepta
        mov  al, 1
        jmp  pnFin
pnOmitir:
        xor  al, al
pnFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        ret
PedirNombre ENDP

; RegistrarPuntaje
;   Recibe:    una partida ganada de 1 jugador: nivel, turno, tiempoFinal,
;              modoPrueba
;   Devuelve:  si entra al top 5 pide las iniciales, inserta el registro
;              y guarda el archivo. puntPuesto = puesto logrado (0..4) o
;              0FFh si no entro, se omitio o es modo prueba.
;   Destruye:  nada
RegistrarPuntaje PROC
        push ax

        mov  puntPuesto, 0FFh
        mov  puntCat, 0         ; categoria 1 jugador
        cmp  modoPrueba, 0
        jne  rpFin              ; con secreto manual el resultado no cuenta
        call BuscarPuesto       ; AL = puesto o 0FFh
        cmp  al, 0FFh
        je   rpFin
        mov  puntPuesto, al
        call PedirNombre        ; AL = 1 si escribio iniciales
        or   al, al
        jz   rpOmitido
        mov  al, puntPuesto
        call InsertarPuntaje
        call GuardarPuntajes
        jmp  rpFin
rpOmitido:
        mov  puntPuesto, 0FFh   ; omitio: no hay record que anunciar
rpFin:
        pop  ax
        ret
RegistrarPuntaje ENDP

; RegistrarDos
;   Recibe:    una partida de 2 jugadores terminada: nivel, dosInt[],
;              dosTiempo[]
;   Devuelve:  si hubo ganador y su resultado entra al top 5 de la
;              categoria 2 jugadores, le pide las iniciales, lo inserta y
;              guarda el archivo. puntPuesto = puesto logrado (0..4) o
;              0FFh si hubo empate, no entro u omitio las iniciales.
;   Destruye:  nada
;   Se guarda el resultado del GANADOR (sus intentos y su tiempo).
RegistrarDos PROC
        push ax
        push bx

        mov  puntPuesto, 0FFh
        cmp  modoPrueba, 0
        jne  rdFin 
        call CompararDos        ; AL = 1 o 2 (ganador), 0 = empate
        or   al, al
        jz   rdFin
        mov  bl, al
        dec  bl
        xor  bh, bh             ; BX = indice del ganador (0 o 1)
        mov  al, dosInt[bx]
        mov  turno, al          ; BuscarPuesto e InsertarPuntaje leen turno...
        push bx
        shl  bx, 1
        mov  ax, dosTiempo[bx]
        mov  tiempoFinal, ax    ; ...y tiempoFinal
        pop  bx
        mov  puntCat, 1         ; categoria 2 jugadores
        call BuscarPuesto
        cmp  al, 0FFh
        je   rdFin
        mov  puntPuesto, al
        mov  ax, bx
        add  al, '1'            ; numero del ganador, 1 o 2
        mov  tRecIntro2 + 8, al
        call PedirNombre
        or   al, al
        jz   rdOmitido
        mov  al, puntPuesto
        call InsertarPuntaje
        call GuardarPuntajes
        jmp  rdFin
rdOmitido:
        mov  puntPuesto, 0FFh
rdFin:
        pop  bx
        pop  ax
        ret
RegistrarDos ENDP

; RegistrarCPU
;   Recibe:    una partida del modo inverso en la que el programa acerto:
;              nivel, turno (intentos que uso), tiempoFinal
;   Devuelve:  si entra al top 5 de la categoria computadora lo inserta
;              con las iniciales CPU (no se piden) y guarda el archivo.
;              puntPuesto = puesto logrado (0..4) o 0FFh si no entro.
;   Destruye:  nada
RegistrarCPU PROC
        push ax

        mov  puntPuesto, 0FFh
        mov  puntCat, 2         ; categoria computadora
        call BuscarPuesto
        cmp  al, 0FFh
        je   rcFin
        mov  puntPuesto, al
        mov  nombreTmp, 'C'     ; la computadora siempre firma CPU
        mov  nombreTmp + 1, 'P'
        mov  nombreTmp + 2, 'U'
        call InsertarPuntaje    ; AL = puesto
        call GuardarPuntajes
rcFin:
        pop  ax
        ret
RegistrarCPU ENDP

; DibujarPuntajes
;   Recibe:    puntCat (categoria que se muestra), selPunt (nivel que se
;              muestra), puntTabla
;   Devuelve:  el nombre de la categoria, el titulo del nivel y sus 5
;              filas (puesto, iniciales, intentos y tiempo); las vacias
;              muestran guiones. Todo se dibuja con fondo propio sobre
;              las barras ya pintadas.
;   Destruye:  nada
DibujarPuntajes PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        mov  ax, C_FONDO
        mov  fondoTexto, ax
        mov  bl, puntCat
        xor  bh, bh
        shl  bx, 1
        mov  si, catPunt[bx]
        mov  ax, 104
        mov  bx, 24
        mov  dl, C_TEXTO
        mov  dh, 1
        call DibujarTexto       ; categoria (14 caracteres: tapa a la anterior)
        mov  bl, selPunt
        xor  bh, bh
        shl  bx, 1
        mov  si, tituloPunt[bx]
        mov  ax, 64
        mov  bx, 34
        mov  dl, C_DORADO
        mov  dh, 2
        call DibujarTexto       ; titulo de 12 caracteres: tapa al anterior

        mov  ax, C_PANEL
        mov  fondoTexto, ax     ; las filas se dibujan sobre sus barras
        mov  puntFila, 0
pfsFila:
        mov  al, selPunt
        call InicioTabla        ; AX = inicio de la tabla de esta categoria y nivel
        mov  di, ax
        mov  al, puntFila
        mov  bl, PUNT_REC
        mul  bl
        add  di, ax             ; DI = registro de esta fila
        mov  al, puntFila
        mov  bl, 20
        mul  bl
        add  ax, 71             ; 66 de la primera barra + 5 para centrar el texto
        mov  puntY, ax

        mov  al, puntFila
        inc  al
        mov  cx, 40
        mov  bx, puntY
        mov  dl, C_DORADO
        mov  dh, 1
        call DibujarNumero      ; numero de puesto

        cmp  BYTE PTR puntTabla[di + 3], 0
        je   pfsVacia

        mov  al, puntTabla[di]
        mov  nomBuf, al
        mov  al, puntTabla[di + 1]
        mov  nomBuf + 1, al
        mov  al, puntTabla[di + 2]
        mov  nomBuf + 2, al
        mov  si, OFFSET nomBuf
        mov  ax, 72
        mov  bx, puntY
        mov  dl, C_TEXTO
        mov  dh, 1
        call DibujarTexto       ; iniciales

        mov  al, puntTabla[di + 3]
        mov  cx, 164
        mov  bx, puntY
        mov  dl, C_DORADO
        call DibujarNumero      ; intentos usados

        mov  ax, WORD PTR puntTabla[di + 4]
        push di
        mov  di, OFFSET tiempoPunt
        call FormatearTiempo    ; AX segundos -> "mm:ss"
        pop  di
        mov  si, OFFSET tiempoPunt
        mov  ax, 236
        mov  bx, puntY
        mov  dl, C_TEXTO
        call DibujarTexto
        jmp  pfsSiguiente
pfsVacia:
        TEXTO tPuntVNom, 72, puntY, C_TEXTO, 1
        TEXTO tPuntVInt, 164, puntY, C_TEXTO, 1
        TEXTO tPuntVTiempo, 236, puntY, C_TEXTO, 1
pfsSiguiente:
        inc  puntFila
        cmp  puntFila, PUNT_TOP
        jae  pfsTermino
        jmp  pfsFila            ; salto cercano: el cuerpo del ciclo es largo
pfsTermino:
        mov  fondoTexto, 0FFFFh

        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
DibujarPuntajes ENDP

; PantallaPuntajes
;   Recibe:    nivel (se empieza mostrando el nivel vigente), puntTabla
;   Devuelve:  vuelve al menu con ESC o ENTER. Las flechas izquierda y
;              derecha cambian el nivel que se muestra; arriba y abajo,
;              la categoria (1 jugador, 2 jugadores, computadora).
;   Destruye:  nada
PantallaPuntajes PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        mov  al, nivel
        mov  selPunt, al
        mov  puntCat, 0         ; siempre se empieza en la categoria 1 jugador
        mov  si, OFFSET tHdrPunt
        mov  di, OFFSET tBarraPunt
        call PintarPantalla

        ; Parte fija de la pantalla: flechas, encabezados y barras de fondo
        TEXTO tPuntArrA, 88, 24, C_DORADO, 1
        TEXTO tPuntArrB, 224, 24, C_DORADO, 1
        TEXTO tPuntIz, 40, 34, C_DORADO, 2
        TEXTO tPuntDe, 264, 34, C_DORADO, 2
        TEXTO tColNum, 40, 54, C_DORADO, 1
        TEXTO tColNombre, 72, 54, C_DORADO, 1
        TEXTO tColInt, 148, 54, C_DORADO, 1
        TEXTO tColTiempo, 236, 54, C_DORADO, 1
        xor  cl, cl             ; CL = barra 0..4
ppBarra:
        mov  al, cl
        mov  bl, 20
        mul  bl
        add  ax, 66
        mov  bx, ax             ; BX = y de la barra
        RELLENAR_RECT C_PANEL, 32, bx, 256, 18
        inc  cl
        cmp  cl, PUNT_TOP
        jb   ppBarra
        CENTRAR tPuntNota, 172, C_TEXTO, 1
        cmp  puntError, 0
        je   ppDibujar
        CENTRAR tPuntErr, 180, C_ROJO, 1
ppDibujar:
        call DibujarPuntajes
        call VaciarTeclado
ppLeer:
        call EsperarTecla
        call TraducirTecla
        cmp  al, ACC_SALIR
        je   ppFin
        cmp  al, ACC_CONFIRMAR
        je   ppFin
        cmp  al, ACC_ARRIBA
        je   ppCatArriba
        cmp  al, ACC_ABAJO
        je   ppCatAbajo
        cmp  al, ACC_IZQ
        je   ppAnterior
        cmp  al, ACC_DER
        jne  ppLeer             ; cualquier otra tecla: ignorar
        mov  al, selPunt        ; derecha: nivel siguiente, da la vuelta
        inc  al
        cmp  al, 3
        jb   ppCambiar
        xor  al, al
        jmp  ppCambiar
ppAnterior:
        mov  al, selPunt
        or   al, al
        jnz  ppRestar
        mov  al, 3
ppRestar:
        dec  al
ppCambiar:
        mov  selPunt, al
        call DibujarPuntajes
        jmp  ppLeer
ppCatAbajo:
        mov  al, puntCat        ; abajo: categoria siguiente, da la vuelta
        inc  al
        cmp  al, PUNT_CATS
        jb   ppCatFijar
        xor  al, al
        jmp  ppCatFijar
ppCatArriba:
        mov  al, puntCat
        or   al, al
        jnz  ppCatRestar
        mov  al, PUNT_CATS
ppCatRestar:
        dec  al
ppCatFijar:
        mov  puntCat, al
        call DibujarPuntajes
        jmp  ppLeer
ppFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
PantallaPuntajes ENDP
