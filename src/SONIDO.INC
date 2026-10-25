
;=====================================================================
; SONIDO.INC - Efectos de sonido con el altavoz interno (opcional)
; El altavoz del PC lo maneja el canal 2 del temporizador 8253/8254:
;   puerto 43h = registro de modo, puerto 42h = divisor del canal 2
;   puerto 61h = bit 0 conecta el canal 2 al altavoz, bit 1 lo enciende
; La frecuencia que suena es 1 193 180 Hz / divisor.
;
; Un sonido es una tabla de notas (DW divisor, DW duracion en tics) que
; termina con un divisor 0. Las notas NO bloquean el juego: IniciarSonido
; enciende la primera y ActualizarSonido, llamado dentro de los ciclos de
; espera de tecla, pasa a la siguiente cuando vence su duracion. La
; duracion se mide con los tics del BIOS, igual que las animaciones.
;=====================================================================

;---------------------------------------------------------------------
; ProgramarTono
;   Recibe:    AX = divisor del temporizador (frecuencia = 1193180 / AX)
;   Devuelve:  el altavoz suena con esa frecuencia
;   Destruye:  nada
;---------------------------------------------------------------------
ProgramarTono PROC
        push ax
        mov  al, 0B6h           ; canal 2, escribir byte bajo y alto, onda cuadrada
        out  43h, al
        pop  ax
        push ax
        out  42h, al            ; byte bajo del divisor
        mov  al, ah
        out  42h, al            ; byte alto del divisor
        in   al, 61h
        or   al, 3              ; bit 0: canal 2 al altavoz; bit 1: altavoz encendido
        out  61h, al
        pop  ax
        ret
ProgramarTono ENDP

;---------------------------------------------------------------------
; ApagarAltavoz
;   Recibe:    nada
;   Devuelve:  el altavoz en silencio (bits 0 y 1 del puerto 61h en 0)
;   Destruye:  nada
;---------------------------------------------------------------------
ApagarAltavoz PROC
        push ax
        in   al, 61h
        and  al, 0FCh
        out  61h, al
        pop  ax
        ret
ApagarAltavoz ENDP

;---------------------------------------------------------------------
; SiguienteNota
;   Recibe:    sonPtr = nota que toca (0 = no hay sonido)
;   Devuelve:  la nota sonando y sonPtr avanzado; si la tabla termino,
;              silencio y sonPtr = 0
;   Destruye:  nada
;---------------------------------------------------------------------
SiguienteNota PROC
        push ax
        push si
        mov  si, sonPtr
        mov  ax, [si]           ; divisor de la nota
        or   ax, ax
        jz   snFin              ; divisor 0 = fin de la tabla
        call ProgramarTono
        mov  ax, [si+2]
        mov  sonDur, ax         ; duracion en tics
        call ObtenerTics
        mov  sonIni, ax         ; instante en que empezo la nota
        add  sonPtr, 4          ; cada nota ocupa 4 bytes
        jmp  snSalir
snFin:
        call ApagarAltavoz
        mov  sonPtr, 0
snSalir:
        pop  si
        pop  ax
        ret
SiguienteNota ENDP

;---------------------------------------------------------------------
; IniciarSonido
;   Recibe:    SI = tabla de notas (DW divisor, DW tics ... DW 0)
;   Devuelve:  empieza a sonar; reemplaza al sonido anterior si lo habia
;   Destruye:  nada
;---------------------------------------------------------------------
IniciarSonido PROC
        mov  sonPtr, si
        call SiguienteNota
        ret
IniciarSonido ENDP

;---------------------------------------------------------------------
; ActualizarSonido
;   Recibe:    nada (se llama dentro de los ciclos de espera de tecla)
;   Devuelve:  pasa a la siguiente nota cuando vence la actual
;   Destruye:  nada
;---------------------------------------------------------------------
ActualizarSonido PROC
        push ax
        cmp  sonPtr, 0
        je   asFin              ; no hay sonido en curso
        call ObtenerTics
        sub  ax, sonIni         ; tics desde que empezo la nota
        cmp  ax, sonDur
        jb   asFin              ; la nota todavia no termina
        call SiguienteNota
asFin:
        pop  ax
        ret
ActualizarSonido ENDP

;---------------------------------------------------------------------
; DetenerSonido
;   Recibe:    nada
;   Devuelve:  silencio inmediato y ningun sonido pendiente
;   Destruye:  nada
;---------------------------------------------------------------------
DetenerSonido PROC
        mov  sonPtr, 0
        call ApagarAltavoz
        ret
DetenerSonido ENDP