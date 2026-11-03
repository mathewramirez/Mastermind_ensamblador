# Mastermind "Símbolos de Costa Rica"

**EIF205 Arquitectura de Computadores - Proyecto II**
Universidad Nacional de Costa Rica, Sede Regional Brunca - II Ciclo 2026

**Integrantes:** Mathew Ramírez - Alen Cedeño

Versión gráfica del juego Mastermind (Toros y Vacas) escrita en **ensamblador x86 de 16 bits** (TASM), que corre en **DOSBox** en modo gráfico VGA 13h (320x200, 256 colores). La temática son símbolos de Costa Rica: las fichas son 8 sprites originales dibujados en Aseprite, además de los sprites de la mascota, el cursor, los iconos de exacto y parcial y las animaciones de victoria y derrota. No se usan bibliotecas gráficas ni lenguajes de alto nivel para la lógica ni el dibujo; Python solo se usó como herramienta auxiliar para convertir los sprites en tablas de bytes.

## Estructura del proyecto

```
/src                     código fuente: MASTER.ASM + un .INC por módulo
/bin                     ejecutable final entregado (MASTER.EXE)
/sprites                 sprites originales (.ase de Aseprite)
/herramientas            sprites2asm.py + paleta.json (solo diseño; no es parte del juego)
/doc                     documento técnico, bitácora, uso de IA, hoja de sprites, tabla de paleta y tablas de bytes
/Turbo Assembler (TASM)  TASM, TLINK y TD del curso
BUILD.bat                script de ensamblado y ejecución del curso
ABRIR_DOSBOX.bat         abre DOSBox con todo montado, para trabajar a mano
```

### Módulos de `/src`

| Archivo | Responsabilidad |
|---|---|
| `MASTER.ASM` | Programa principal: flujo entre pantallas y salida a DOS |
| `CONST.INC` | Constantes: zonas de pantalla, colores, teclas, límites |
| `DATOS.INC` | Variables y tablas (secreto, intento, historial, geometría por nivel) |
| `TEXTOS.INC` | Cadenas de texto de todas las pantallas |
| `MACROS.INC` | Macros: `DIBUJAR_SPRITE`, `RELLENAR_RECT`, `TEXTO`, `CENTRAR`, `LIMPIAR_ARREGLO` |
| `VIDEO.INC` | Modo de vídeo, paleta (puertos 3C8h/3C9h), `DibujarSprite`, `DibujarSpriteX2`, píxeles |
| `PALETA.INC` | Tabla de paleta (generada por `sprites2asm.py`) |
| `SPRITES.INC` | Datos de los sprites (generado por `sprites2asm.py`) |
| `TEXTO.INC` | Rectángulos y texto con la fuente de 8x8 del BIOS |
| `ANIM.INC` | Animación de 2 cuadros controlada por el contador de tics del BIOS |
| `TECLADO.INC` | Lectura de teclas con INT 16h |
| `ENTRADA.INC` | Traducción de teclas a acciones y edición del intento |
| `ALEAT.INC` | Generador pseudoaleatorio sembrado con INT 1Ah y código secreto |
| `JUEGO.INC` | Niveles, reinicio de partida y evaluación de toros y vacas |
| `CICLO.INC` | Turnos, historial, victoria y derrota |
| `TABLERO.INC` | Dibujo del tablero de intentos y la retroalimentación |
| `PANEL.INC` | Panel lateral: fichas, intento actual y contadores |
| `PARTIDA.INC` | Partida, modo de prueba, confirmación al salir y pantalla final |
| `PANTALLA.INC` | Presentación, menú, dificultad, instrucciones y créditos |
| `CRONO.INC` | Cronómetro de la partida con el contador de tics del BIOS |
| `SONIDO.INC` | Efectos de sonido con el altavoz interno (puertos 61h y 43h) |
| `FADE.INC` | Transiciones de pantalla con cambio progresivo de la paleta |
| `PUNTAJES.INC` | Mejores puntajes guardados en `MEJORES.DAT` |
| `DOSJUG.INC` | Modo de dos jugadores por turnos |
| `INVERSO.INC` | Modo inverso: el programa deduce el código del jugador |

## Requisitos

- Windows con **DOSBox 0.74-3** (instalado en `C:\Program Files (x86)\DOSBox-0.74-3\` o en una carpeta `DOSBox-0.74-3` dentro del proyecto).
- El **TASM del curso** en la carpeta `Turbo Assembler (TASM)\TASM` del proyecto (`TASM.EXE`, `TLINK.EXE`, `TD.EXE`).

## Ensamblar, enlazar y ejecutar

### Opción 1: con BUILD.bat (recomendada)

Desde una terminal de Windows (`cmd`) en la raíz del proyecto:

```
BUILD.bat src\MASTER
```

Ensambla, enlaza y ejecuta el juego en DOSBox, y deja `MASTER.EXE` en la carpeta `src`. Si el ensamblado o el enlace fallan, DOSBox se cierra. Para depurar con Turbo Debugger:

```
BUILD.bat src\MASTER debug
```

### Opción 2: a mano, dentro de DOSBox

Haz doble clic en `ABRIR_DOSBOX.bat`. Abre DOSBox con la carpeta `src` montada como `C:` y las herramientas de TASM en `T:`. Dentro de DOSBox escribe:

```
dir *.inc
tasm master
tlink master
master
```

El primer comando solo lista los `.INC` para comprobar que están todos antes de ensamblar. El ensamblado debe terminar con `0 errores` y `0 advertencias`.

> Siempre se ensambla `master`, nunca un `.INC` suelto: los `.INC` se incluyen desde `MASTER.ASM`.

### Opción 3: ejecutar el juego ya compilado

`bin\MASTER.EXE` es el ejecutable entregado, generado con el código de `/src`. No hace falta ensamblar. Abre DOSBox y escribe (cambia `<ruta del proyecto>` por la carpeta donde está el proyecto):

```
mount c "<ruta del proyecto>\bin"
c:
MASTER
```

Los cuatro casos de la Tabla 2 del enunciado se prueban dentro del juego con el modo de prueba (ver más abajo).

## Cómo se juega

El objetivo es descubrir el código secreto de símbolos. Tras cada intento el juego muestra, en una fila de iconos junto al intento:

- **Toro** (icono de exacto): símbolo correcto en la posición correcta.
- **Vaca** (icono de parcial): símbolo correcto en una posición equivocada.

Primero van los iconos de exacto y luego los de parcial. Un símbolo del secreto solo se cuenta una vez.

### Menú principal

| Opción | Qué hace |
|---|---|
| Jugar | Empieza una partida con el nivel y el número de jugadores elegidos |
| Dificultad | Elige Fácil, Normal o Difícil |
| Jugadores | Alterna entre 1 y 2 jugadores (ENTER o ← →) |
| Modo inverso | El jugador piensa un código y el programa lo deduce |
| Mejores puntajes | Muestra el top 5 por categoría y nivel |
| Instrucciones | Reglas, niveles, controles, dos jugadores y modo inverso (5 páginas) |
| Créditos | Integrantes y curso |
| Salir | Restaura el modo de vídeo y regresa a DOS |

En el menú, ↑ ↓ mueven la selección y ENTER elige.

### Niveles

| Nivel | Posiciones | Símbolos | Repetidos | Intentos |
|---|---|---|---|---|
| Fácil | 4 | 6 | No | 12 |
| Normal | 4 | 6 | Sí | 10 |
| Difícil | 5 | 8 | Sí | 10 |

### Controles de la partida

| Tecla | Acción |
|---|---|
| ← → | Cambiar la posición activa |
| ↑ ↓ | Cambiar el símbolo de la posición activa |
| 1 a 6 (u 8 en Difícil) | Poner ese símbolo directamente |
| ENTER | Confirmar el intento (solo si está completo; si no, no cuenta como turno) |
| BACKSPACE | Borrar la posición activa |
| ESC | Pide confirmación para abandonar la partida; con S se sale al menú y se pierde lo jugado |

Al terminar una partida, ENTER juega otra vez y ESC o M vuelven al menú.

## Modo de dos jugadores

1. En el menú, pon **Jugadores: 2** (ENTER o ← →) y elige **Jugar**.
2. Son dos rondas con el mismo nivel. En cada ronda un jugador escribe el secreto sin que el otro mire y el otro intenta adivinarlo. En la ronda 2 se cambian los papeles. El encabezado indica de quién es el turno (`TURNO: J1` o `TURNO: J2`).
3. Gana quien adivine con menos intentos y, si empatan, quien tarde menos. Quien no adivina cuenta como el peor resultado.
4. El ganador entra a Mejores puntajes (categoría 2 JUGADORES) y escribe sus iniciales. Un empate no guarda récord.

## Modo inverso

1. En el menú elige **Modo inverso**.
2. Escribe en el panel el código que vas a pensar, con las teclas de los símbolos (BACKSPACE borra, ENTER empieza, ESC vuelve al menú). En Fácil no se aceptan símbolos repetidos. Ese código queda a la vista en la pantalla para que no lo olvides.
3. El programa propone un intento. Tú respondes cuántos exactos y cuántos parciales tiene: ↑ ↓ cambian el número, ← → cambian el campo, también puedes escribir el dígito directamente, y ENTER confirma.
4. Antes de aceptar tu respuesta, el programa la compara con los exactos y parciales reales de tu código. Si no coincide, suena un aviso (`Eso no coincide con tu código: corrige`), no se dibuja nada en el tablero, no se gasta intento y puedes corregir los números.
5. Con tus respuestas correctas, el programa descarta todos los códigos que no son compatibles y propone el siguiente que sí lo es.

El código que escribes **no se usa para deducir**: el programa solo usa tus respuestas. Se usa únicamente para detectar si te equivocaste al responder. ESC durante la partida pide confirmación antes de abandonar.

Si el programa lo adivina, queda en la categoría COMPUTADORA de Mejores puntajes con las siglas CPU, sin pedir iniciales.

## Mejores puntajes

- Se guardan en el archivo `MEJORES.DAT`, en la carpeta desde la que se ejecuta el juego. Si no existe, se crea solo.
- Hay tres categorías (1 JUGADOR, 2 JUGADORES y COMPUTADORA) y tres niveles; cada combinación guarda 5 récords.
- Se ordenan por menos intentos y, luego, por menos tiempo.
- En la pantalla de Mejores puntajes, ↑ ↓ cambian la categoría y ← → el nivel.
- Las partidas en modo de prueba no cuentan.

## Modo de prueba (secreto manual)

Sirve para ejecutar los casos de la Tabla 2 del enunciado con un secreto conocido.

1. En el menú principal pulsa **T**. Aparece `[MODO PRUEBA]` en rojo; pulsar T o t otra vez lo desactiva.
2. Elige el nivel en **Dificultad** (usa **Normal** para los casos con símbolos repetidos).
3. Elige **Jugar**. El juego pide el secreto: escribe las teclas de los símbolos. En Fácil no acepta repetidos. ESC en esta pantalla deja el secreto aleatorio.
4. Juega con normalidad y compara los iconos con el resultado esperado.

Las teclas corresponden a R=1, V=2, A=3, M=4, N=5, B=6.

| Caso | Secreto | Intento | Resultado esperado | Teclas del secreto | Teclas del intento |
|---|---|---|---|---|---|
| 1 | RVAM | RAVM | 2 toros, 2 vacas | 1 2 3 4 | 1 3 2 4 |
| 2 | RRVA | RVRR | 1 toro, 2 vacas | 1 1 2 3 | 1 2 1 1 |
| 3 | RVAM | NBNB | 0 toros, 0 vacas | 1 2 3 4 | 5 6 5 6 |
| 4 | AAAA | AARR | 2 toros, 0 vacas | 3 3 3 3 | 3 3 1 1 |

## Regenerar sprites y paleta

Después de editar un sprite en `/sprites`:

```
pip install pillow
python herramientas\sprites2asm.py
```

Genera `src\SPRITES.INC`, `src\PALETA.INC`, `doc\hoja_sprites.png` y `doc\tabla_paleta.md`. Luego hay que volver a ensamblar el proyecto.

## Funcionalidades opcionales implementadas

- Contador de tiempo de la partida (INT 1Ah), visible en el encabezado.
- Mejores puntajes en archivo (INT 21h funciones 3Ch, 3Dh, 3Fh, 40h y 3Eh).
- Efectos de sonido con el altavoz interno (puerto 61h y temporizador en el puerto 43h).
- Modo inverso con estrategia de descarte.
- Modo de dos jugadores por turnos en la misma máquina.
- Transiciones de pantalla con fundido de paleta.

## Notas técnicas

- **Vídeo:** modo 13h por INT 10h, dibujo directo en `A000h`. La pantalla se redibuja de forma incremental (solo lo que cambia) para evitar el parpadeo.
- **Tablero:** la geometría de cada nivel está en la tabla `tablaGeom` de `DATOS.INC` (Fácil, Normal, Difícil, en ese orden). Normal y Difícil usan una columna de 10 filas; Fácil usa 2 columnas de 6 filas para que quepan los 12 intentos a la vez en los 168 px de alto del tablero.
- **Sprites:** una sola rutina `DibujarSprite` (datos, ancho, alto, x, y); el índice de color 0 es transparente. Los iconos de exacto y parcial son de 8x8: en Fácil se dibujan tal cual, en dos líneas, y en Normal y Difícil se dibujan al doble con `DibujarSpriteX2`.
- **Paleta:** 56 colores propios desde el índice 16, cargados con `OUT` a los puertos `3C8h` y `3C9h`. La tabla está en `doc\tabla_paleta.md`.
- **Animaciones:** mascota de presentación, cursor de la casilla activa y animaciones de victoria y derrota. Su velocidad depende del contador de tics del BIOS (INT 1Ah), no de la CPU.
- **Aleatoriedad:** generador congruencial lineal sembrado con INT 1Ah función 00h.
- **Teclado:** INT 16h, con las teclas extendidas distinguidas por su código de exploración.
- **Rendimiento:** en el modo inverso con nivel Difícil el programa hace muchos cálculos para buscar el siguiente código. Si en tu equipo tarda demasiado, sube los ciclos de DOSBox con Ctrl+F12.
- **Salida:** se restaura el modo de vídeo original y se regresa a DOS con INT 21h función 4Ch.

