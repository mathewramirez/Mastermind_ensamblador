@echo off
rem Abre DOSBox para trabajar a mano: C: = carpeta src, T: = TASM del curso
rem Dentro de DOSBox:  tasm master  /  tlink master  /  master
setlocal
set "ROOT=%~dp0"
set "DBX=%ROOT%DOSBox-0.74-3\DOSBox.exe"
if not exist "%DBX%" set "DBX=C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe"
set "TOOLS=%ROOT%Turbo Assembler (TASM)\TASM"
start "" "%DBX%" -c "mount c \"%ROOT%src\"" -c "mount t \"%TOOLS%\"" -c "c:" -c "set PATH=Z:\;T:"
endlocal
