# Mastermind "Simbolos de Costa Rica" - EIF205 Proyecto II

Ensamblador x86 de 16 bits, modo 13h. UNA Sede Regional Brunca - II Ciclo 2026.
Integrantes: _(nombre 1)_ - _(nombre 2)_

## Estructura

```
/src                     codigo fuente (MASTER.ASM + un .INC por modulo)
/bin                     ejecutable final entregado (MASTER.EXE)
/sprites                 sprites originales (.ase de Aseprite)
/herramientas            sprites2asm.py + paleta.json (diseno; no es parte del juego)
/doc                     documento tecnico, bitacora, hoja de sprites, tabla de paleta, uso de IA
/Turbo Assembler (TASM)  TASM, TLINK, TD del curso
BUILD.bat                script de ensamblado del curso
```

## Ensamblar, enlazar y ejecutar

Desde una terminal de Windows (cmd) en la raiz del proyecto:

```
BUILD.bat src\MASTER
```

Ensambla, enlaza y ejecuta en DOSBox. Para depurar con Turbo Debugger:

```
BUILD.bat src\MASTER debug
```

Manual, dentro de DOSBox (o con `ABRIR_DOSBOX.bat`, que ya monta todo):

```
mount c "<ruta del proyecto>\src"
mount t "<ruta del proyecto>\Turbo Assembler (TASM)\TASM"
c:
t:\tasm master.asm
t:\tlink master.obj
master
```

## Regenerar sprites y paleta

Despues de editar algun sprite en `/sprites`:

```
pip install pillow
python herramientas\sprites2asm.py
```

Genera `src/SPRITES.INC`, `src/PALETA.INC`, `doc/hoja_sprites.png` y `doc/tabla_paleta.md`.
