;=====================================================================
; PRUEBA2.ASM - Pruebas de la Fase 2 (modo texto)
; Herramienta de desarrollo: NO es parte del juego entregado.
;   1) Los 4 casos oficiales de la Tabla 2 del enunciado
;   2) Generacion de secretos: 500 por nivel, validados automaticamente
;   3) Prueba manual: el usuario escribe secreto e intento
; Ensamblar:  BUILD.bat src\PRUEBA2
;
; Las letras R V A M N B son las de la Tabla 2: el enunciado aclara
; que solo son identificadores de color. Se convierten a IDs 1..6.
; exactos = "toros" del enunciado, parciales = "vacas".
;=====================================================================
        .MODEL SMALL
        .286
        .STACK 200h

        INCLUDE CONST.INC
        INCLUDE MACROS.INC

        .DATA
        INCLUDE SPRITES.INC     ; DATOS.INC usa sus etiquetas (tablaFichas)
        INCLUDE DATOS.INC

letras      DB 'RVAMNB'         ; posicion+1 = ID: R=1 V=2 A=3 M=4 N=5 B=6
; Tabla 2: secreto(4), intento(4), exactos esperados, parciales esperados
casos       DB 'RVAM','RAVM',2,2
            DB 'RRVA','RVRR',1,2
            DB 'RVAM','NBNB',0,0
            DB 'AAAA','AARR',2,0

msgTitulo   DB 'PRUEBAS FASE 2 - Mastermind Simbolos de Costa Rica',13,10,'$'
msgCasos    DB '--- Casos oficiales (Tabla 2) ---',13,10,'$'
msgCaso     DB 'Caso $'
msgObt      DB ': exactos=$'
msgVac      DB ' parciales=$'
msgOk       DB '  OK',13,10,'$'
msgFalla    DB '  FALLA',13,10,'$'
msgGen      DB '--- Generacion de secretos (0=facil 1=normal 2=dificil) ---',13,10,'$'
msgNivel    DB 'Nivel $'
msgInval    DB ': invalidos de 500 = $'
msgEj       DB '   ejemplo: $'
msgManual   DB '--- Prueba manual (letras R V A M N B, 4 posiciones) ---',13,10,'$'
msgPideS    DB 'Secreto: $'
msgPideI    DB 'Intento: $'
msgResult   DB 'Resultado: exactos=$'
msgOtra     DB 'Otra prueba? (S/N): $'

        .CODE
inicio:
        mov  ax, @data
        mov  ds, ax
        cld

        mov  dx, OFFSET msgTitulo
        call ImprimirCadena
        call CorrerCasos
        call VerificarGeneracion
        call ModoManual

        mov  ax, 4C00h
        int  21h

;---------------------------------------------------------------------
; ImprimirCadena
;   Recibe:    DX = offset de una cadena terminada en '$'
;   Devuelve:  nada
;   Destruye:  nada
;---------------------------------------------------------------------
ImprimirCadena PROC
        push ax
        mov  ah, 09h
        int  21h
        pop  ax
        ret
ImprimirCadena ENDP

;---------------------------------------------------------------------
; ImprimirDigito
;   Recibe:    DL = valor 0..9
;   Devuelve:  nada (imprime el caracter)
;   Destruye:  nada
;---------------------------------------------------------------------
ImprimirDigito PROC
        push ax
        push dx
        add  dl, '0'
        mov  ah, 02h
        int  21h
        pop  dx
        pop  ax
        ret
ImprimirDigito ENDP

;---------------------------------------------------------------------
; ImprimirNumero
;   Recibe:    AX = numero sin signo
;   Devuelve:  nada (imprime en decimal)
;   Destruye:  nada
; Divide entre 10 repetidamente; los restos salen en orden inverso,
; por eso se guardan en la pila y se sacan despues.
;---------------------------------------------------------------------
ImprimirNumero PROC
        push ax
        push bx
        push cx
        push dx
        mov  bx, 10
        xor  cx, cx             ; CX = cuantos digitos
inDiv:
        xor  dx, dx
        div  bx                 ; AX = cociente, DX = resto (un digito)
        push dx
        inc  cx
        or   ax, ax
        jnz  inDiv
inSacar:
        pop  dx
        add  dl, '0'
        mov  ah, 02h
        int  21h
        loop inSacar
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
ImprimirNumero ENDP

;---------------------------------------------------------------------
; NuevaLinea
;   Recibe: nada.  Devuelve: nada.  Destruye: nada
;---------------------------------------------------------------------
NuevaLinea PROC
        push ax
        push dx
        mov  ah, 02h
        mov  dl, 13
        int  21h
        mov  ah, 02h
        mov  dl, 10
        int  21h
        pop  dx
        pop  ax
        ret
NuevaLinea ENDP

;---------------------------------------------------------------------
; LetraAId
;   Recibe:    AL = letra (mayuscula o minuscula)
;   Devuelve:  AL = ID de simbolo 1..6, o 0 si la letra no es valida
;   Destruye:  AH
;---------------------------------------------------------------------
LetraAId PROC
        push cx
        push di
        and  al, 0DFh           ; pasa a mayuscula (apaga el bit 5)
        mov  cx, 6
        xor  di, di
laBuscar:
        cmp  al, letras[di]
        je   laHallada
        inc  di
        loop laBuscar
        xor  al, al             ; no esta en la tabla
        jmp  laFin
laHallada:
        mov  ax, di
        inc  al                 ; ID = posicion + 1
laFin:
        pop  di
        pop  cx
        ret
LetraAId ENDP

;---------------------------------------------------------------------
; CargarLetras
;   Recibe:    SI = texto con numPos letras, DI = arreglo destino
;   Devuelve:  el arreglo con los IDs de simbolo
;   Destruye:  nada
;---------------------------------------------------------------------
CargarLetras PROC
        push ax
        push cx
        push si
        push di
        mov  cl, numPos
        xor  ch, ch
clLetra:
        mov  al, [si]
        call LetraAId
        mov  [di], al
        inc  si
        inc  di
        loop clLetra
        pop  di
        pop  si
        pop  cx
        pop  ax
        ret
CargarLetras ENDP

;---------------------------------------------------------------------
; LeerCodigo
;   Recibe:    DI = arreglo destino; numPos
;   Devuelve:  el arreglo lleno con IDs. Letras invalidas se ignoran
;   Destruye:  nada
;---------------------------------------------------------------------
LeerCodigo PROC
        push ax
        push cx
        push di
        mov  cl, numPos
        xor  ch, ch
lcLetra:
        mov  ah, 01h
        int  21h                ; AL = tecla pulsada (con eco)
        call LetraAId
        or   al, al
        jz   lcLetra            ; invalida: pedirla otra vez
        mov  [di], al
        inc  di
        loop lcLetra
        call NuevaLinea
        pop  di
        pop  cx
        pop  ax
        ret
LeerCodigo ENDP

;---------------------------------------------------------------------
; SecretoValido
;   Recibe:    secreto, numPos, numColores, repeticion
;   Devuelve:  AL = 1 si cada simbolo esta en 1..C y, si repeticion=0,
;              ninguno se repite; AL = 0 en caso contrario
;   Destruye:  AX
;---------------------------------------------------------------------
SecretoValido PROC
        push cx
        push si
        push di
        mov  cl, numPos
        xor  ch, ch
        xor  si, si
svPos:
        mov  al, secreto[si]
        cmp  al, 1
        jb   svMal              ; 0 = casilla vacia
        cmp  al, numColores
        ja   svMal              ; simbolo que no existe en este nivel
        cmp  repeticion, 0
        jne  svSig              ; con repeticion basta el rango
        xor  di, di
svComp:
        cmp  di, si
        jae  svSig
        cmp  al, secreto[di]
        je   svMal              ; repetido en nivel facil
        inc  di
        jmp  svComp
svSig:
        inc  si
        loop svPos
        mov  al, 1
        jmp  svFin
svMal:
        xor  al, al
svFin:
        pop  di
        pop  si
        pop  cx
        ret
SecretoValido ENDP

;---------------------------------------------------------------------
; CorrerCasos
;   Ejecuta los 4 casos de la Tabla 2 y compara con lo esperado.
;   Recibe: nada.  Devuelve: nada (imprime).  Destruye: nada
;---------------------------------------------------------------------
CorrerCasos PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        mov  dx, OFFSET msgCasos
        call ImprimirCadena
        mov  numPos, 4          ; los 4 casos son de 4 posiciones
        mov  si, OFFSET casos   ; SI = inicio del caso actual
        mov  cx, 4              ; cantidad de casos
        mov  bx, 1              ; BX = numero de caso (para mostrarlo)
ccSiguiente:
        mov  di, OFFSET secreto
        call CargarLetras       ; secreto = las 4 letras apuntadas por SI
        add  si, 4
        mov  di, OFFSET intento
        call CargarLetras
        add  si, 4              ; SI -> exactos esperados
        call EvaluarIntento     ; AL = exactos, AH = parciales

        mov  dx, OFFSET msgCaso
        call ImprimirCadena
        mov  dl, bl
        call ImprimirDigito
        mov  dx, OFFSET msgObt
        call ImprimirCadena
        mov  dl, al
        call ImprimirDigito
        mov  dx, OFFSET msgVac
        call ImprimirCadena
        mov  dl, ah
        call ImprimirDigito

        cmp  al, [si]           ; exactos esperados
        jne  ccFalla
        cmp  ah, [si+1]         ; parciales esperados
        jne  ccFalla
        mov  dx, OFFSET msgOk
        jmp  ccMostrar
ccFalla:
        mov  dx, OFFSET msgFalla
ccMostrar:
        call ImprimirCadena
        add  si, 2              ; siguiente caso
        inc  bx
        dec  cx
        jz   ccFin
        jmp  ccSiguiente        ; LOOP no alcanza: el cuerpo pasa de 128 bytes
ccFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
CorrerCasos ENDP

;---------------------------------------------------------------------
; VerificarGeneracion
;   Para cada nivel genera 500 secretos con IniciarPartida y cuenta
;   los invalidos (fuera de rango o, en Facil, con repetidos).
;   Recibe: nada.  Devuelve: nada (imprime).  Destruye: nada
;---------------------------------------------------------------------
VerificarGeneracion PROC
        push ax
        push bx
        push cx
        push dx
        push si
        push di

        mov  dx, OFFSET msgGen
        call ImprimirCadena
        xor  bx, bx             ; BL = nivel que se prueba
vgNivel:
        mov  nivel, bl
        mov  cx, 500
        xor  si, si             ; SI = cuantos secretos salieron invalidos
vgPartida:
        call IniciarPartida     ; configura, limpia y genera un secreto
        call SecretoValido
        or   al, al
        jnz  vgBien
        inc  si
vgBien:
        loop vgPartida

        mov  dx, OFFSET msgNivel
        call ImprimirCadena
        mov  dl, bl
        call ImprimirDigito
        mov  dx, OFFSET msgInval
        call ImprimirCadena
        mov  ax, si
        call ImprimirNumero
        mov  dx, OFFSET msgEj
        call ImprimirCadena
        mov  cl, numPos
        xor  ch, ch
        xor  di, di
vgMostrar:
        mov  dl, secreto[di]
        call ImprimirDigito
        inc  di
        loop vgMostrar
        call NuevaLinea

        inc  bx
        cmp  bx, 3
        jae  vgFin
        jmp  vgNivel
vgFin:
        pop  di
        pop  si
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
VerificarGeneracion ENDP

;---------------------------------------------------------------------
; ModoManual
;   Pide secreto e intento por teclado y muestra el resultado.
;   Recibe: nada.  Devuelve: nada.  Destruye: nada
;---------------------------------------------------------------------
ModoManual PROC
        push ax
        push dx
        push di

        mov  numPos, 4
        mov  dx, OFFSET msgManual
        call ImprimirCadena
mmCiclo:
        mov  dx, OFFSET msgPideS
        call ImprimirCadena
        mov  di, OFFSET secreto
        call LeerCodigo
        mov  dx, OFFSET msgPideI
        call ImprimirCadena
        mov  di, OFFSET intento
        call LeerCodigo

        call EvaluarIntento     ; AL = exactos, AH = parciales
        mov  dx, OFFSET msgResult
        call ImprimirCadena
        mov  dl, al
        call ImprimirDigito
        mov  dx, OFFSET msgVac
        call ImprimirCadena
        mov  dl, ah
        call ImprimirDigito
        call NuevaLinea

        mov  dx, OFFSET msgOtra
        call ImprimirCadena
        mov  ah, 01h
        int  21h                ; AL = tecla
        and  al, 0DFh
        cmp  al, 'S'
        jne  mmFin
        call NuevaLinea
        jmp  mmCiclo
mmFin:
        call NuevaLinea
        pop  di
        pop  dx
        pop  ax
        ret
ModoManual ENDP

        INCLUDE ALEAT.INC
        INCLUDE JUEGO.INC

        END inicio