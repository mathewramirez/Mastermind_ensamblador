@echo off
setlocal

rem ============================================
rem Configuration
rem ============================================

set "DOSBOX=C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe"
set "ROOT=%~dp0"
if not exist "%DOSBOX%" set "DOSBOX=%ROOT%DOSBox-0.74-3\DOSBox.exe"
set "TOOLS=%ROOT%Turbo Assembler (TASM)\TASM"

rem ============================================
rem Validate arguments
rem ============================================

if "%~1"=="" goto usage

rem ============================================
rem Find ASM file
rem ============================================

if exist "%~1" goto file_with_extension
if exist "%~1.asm" goto file_without_extension

echo.
echo Error: ASM file not found:
echo %~1
echo.
exit /b 1

:file_with_extension
set "ASM_PATH=%~f1"
goto file_found

:file_without_extension
set "ASM_PATH=%~f1.asm"
goto file_found

:file_found

for %%I in ("%ASM_PATH%") do set "PROJECT=%%~dpI"
for %%I in ("%ASM_PATH%") do set "ASM_NAME=%%~nI"

rem ============================================
rem Select mode
rem ============================================

set "MODE=%~2"

if "%MODE%"=="" set "MODE=run"

if /I "%MODE%"=="run" goto validate
if /I "%MODE%"=="debug" goto validate

echo.
echo Error: Invalid mode "%MODE%"
echo Valid modes: run, debug
echo.
exit /b 1

rem ============================================
rem Validate programs
rem ============================================

:validate

if not exist "%DOSBOX%" goto dosbox_missing
if not exist "%TOOLS%\TASM.EXE" goto tasm_missing
if not exist "%TOOLS%\TLINK.EXE" goto tlink_missing

if /I "%MODE%"=="debug" if not exist "%TOOLS%\TD.EXE" goto debugger_missing

rem ============================================
rem Remove previous build
rem ============================================

del /q "%PROJECT%%ASM_NAME%.obj" 2>nul
del /q "%PROJECT%%ASM_NAME%.exe" 2>nul
del /q "%PROJECT%%ASM_NAME%.map" 2>nul

echo.
echo ============================================
echo ASM:    %ASM_NAME%.asm
echo Folder: %PROJECT%
echo Mode:   %MODE%
echo ============================================
echo.

if /I "%MODE%"=="debug" goto debug_mode
goto run_mode

rem ============================================
rem Run
rem ============================================

:run_mode

"%DOSBOX%" ^
-c "mount c \"%PROJECT%\"" ^
-c "mount t \"%TOOLS%\"" ^
-c "c:" ^
-c "t:\tasm.exe %ASM_NAME%.asm" ^
-c "if errorlevel 1 exit" ^
-c "t:\tlink.exe %ASM_NAME%.obj" ^
-c "if errorlevel 1 exit" ^
-c "%ASM_NAME%.exe"

goto end

rem ============================================
rem Debug
rem ============================================

:debug_mode

"%DOSBOX%" ^
-c "mount c \"%PROJECT%\"" ^
-c "mount t \"%TOOLS%\"" ^
-c "c:" ^
-c "t:\tasm.exe /zi %ASM_NAME%.asm" ^
-c "if errorlevel 1 exit" ^
-c "t:\tlink.exe /v %ASM_NAME%.obj" ^
-c "if errorlevel 1 exit" ^
-c "t:\td.exe %ASM_NAME%.exe"

goto end

rem ============================================
rem Errors
rem ============================================

:usage
echo.
echo Usage:
echo   %~nx0 ^<asm_file^> [run^|debug]
echo.
echo Examples:
echo   %~nx0 .\clase1\class1
echo   %~nx0 .\clase1\class1.asm
echo   %~nx0 .\clase1\class1 debug
echo.
exit /b 1

:dosbox_missing
echo.
echo Error: DOSBox was not found:
echo %DOSBOX%
echo.
exit /b 1

:tasm_missing
echo.
echo Error: TASM.EXE was not found:
echo %TOOLS%\TASM.EXE
echo.
exit /b 1

:tlink_missing
echo.
echo Error: TLINK.EXE was not found:
echo %TOOLS%\TLINK.EXE
echo.
exit /b 1

:debugger_missing
echo.
echo Error: TD.EXE was not found:
echo %TOOLS%\TD.EXE
echo.
exit /b 1

:end
endlocal