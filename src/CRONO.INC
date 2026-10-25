
;=====================================================================
; CRONO.INC - Cronometro de la partida (funcionalidad opcional)
; Cuenta el tiempo con el contador de tics del BIOS (INT 1Ah, 18.2 tics
; por segundo), el mismo reloj que controla las animaciones. Se muestra
; en el encabezado como "Tiempo mm:ss" y se congela al terminar.
;=====================================================================

;---------------------------------------------------------------------
; ObtenerSegundos
;   Recibe:    ticsPartida (fijado por IniciarCrono)
;   Devuelve:  AX = segundos transcurridos = tics * 5 / 91
;              (1 s = 18.2 tics, o sea 91 tics cada 5 s)
;   Destruye:  AX
;   Con 16 bits alcanza para ~1 hora; tics*5 se guarda en DX:AX y el
;   cociente (<= 3600) cabe en AX, asi que DIV no desborda.
;---------------------------------------------------------------------
ObtenerSegundos PROC
        push cx
        push dx
        call ObtenerTics        ; AX = parte baja del contador
        sub  ax, ticsPartida    ; diferencia sin signo (tolera el desborde)
        mov  cx, 5
        mul  cx                 ; DX:AX = tics * 5
        mov  cx, 91
        div  cx                 ; AX = segundos
        pop  dx
        pop  cx
        ret
ObtenerSegundos ENDP

;---------------------------------------------------------------------
; FormatearTiempo
;   Recibe:    AX = segundos, DI = direccion donde escribir "mm:ss"
;   Devuelve:  5 caracteres ASCII en [DI..DI+4] (minutos topados en 99)
;   Destruye:  nada
;---------------------------------------------------------------------
FormatearTiempo PROC
        push ax
        push bx
        push cx
        push dx

        xor  dx, dx
        mov  cx, 60
        div  cx                 ; AX = minutos, DX = segundos
        mov  cx, dx             ; CX = segundos (DX se usa abajo)
        cmp  ax, 99
        jbe  ftMinutos
        mov  ax, 99
ftMinutos:
        mov  bl, 10
        div  bl                 ; AL = decenas de minuto, AH = unidades
        add  ax, '0' + ('0' SHL 8)
        mov  [di], al
        mov  [di+1], ah
        mov  BYTE PTR [di+2], ':'
        mov  ax, cx             ; segundos 0..59
        div  bl                 ; AL = decenas, AH = unidades
        add  ax, '0' + ('0' SHL 8)
        mov  [di+3], al
        mov  [di+4], ah

        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
FormatearTiempo ENDP

;---------------------------------------------------------------------
; DibujarCrono
;   Recibe:    AX = segundos a mostrar
;   Devuelve:  "Tiempo mm:ss" en el encabezado (96, 6), pintado de una
;              sola pasada con el fondo del encabezado (sin parpadeo)
;   Destruye:  nada
;---------------------------------------------------------------------
DibujarCrono PROC
        push ax
        push bx
        push dx
        push si
        push di

        mov  di, OFFSET tiempoBuf + 7
        call FormatearTiempo
        mov  fondoTexto, C_PANEL        ; el encabezado es color C_PANEL
        TEXTO tiempoBuf, 96, 6, C_TEXTO, 1
        mov  fondoTexto, 0FFFFh

        pop  di
        pop  si
        pop  dx
        pop  bx
        pop  ax
        ret
DibujarCrono ENDP

;---------------------------------------------------------------------
; IniciarCrono
;   Recibe:    nada
;   Devuelve:  cronometro en 0 (el encabezado ya muestra 00:00)
;   Destruye:  nada
;---------------------------------------------------------------------
IniciarCrono PROC
        push ax
        call ObtenerTics
        mov  ticsPartida, ax
        mov  segMostrados, 0
        pop  ax
        ret
IniciarCrono ENDP

;---------------------------------------------------------------------
; ActualizarCrono
;   Recibe:    cronometro en marcha
;   Devuelve:  redibuja el tiempo solo cuando cambia el segundo
;   Destruye:  nada
;---------------------------------------------------------------------
ActualizarCrono PROC
        push ax
        call ObtenerSegundos
        cmp  ax, segMostrados
        je   acFin              ; mismo segundo: no se dibuja nada
        mov  segMostrados, ax
        call DibujarCrono
acFin:
        pop  ax
        ret
ActualizarCrono ENDP

;---------------------------------------------------------------------
; FinalizarCrono
;   Recibe:    cronometro en marcha
;   Devuelve:  tiempoFinal = segundos de la partida; el encabezado
;              queda congelado con ese valor
;   Destruye:  nada
;---------------------------------------------------------------------
FinalizarCrono PROC
        push ax
        call ObtenerSegundos
        mov  tiempoFinal, ax
        call DibujarCrono
        pop  ax
        ret
FinalizarCrono ENDP