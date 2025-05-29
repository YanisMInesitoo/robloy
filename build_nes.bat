@echo off
REM Compila Robloy NES automáticamente usando cc65
setlocal

REM === Manejo de argumentos ===
if /I "%1"=="/h" goto :help
if /I "%1"=="/?" goto :help
if /I "%1"=="clean" goto :clean

REM Detectar cl65.exe automáticamente
set "CC65_BIN="
if exist "%CD%\..\cc65\bin\cl65.exe" set "CC65_BIN=%CD%\..\cc65\bin"
if exist "%CD%\cc65\bin\cl65.exe" set "CC65_BIN=%CD%\cc65\bin"
if exist "%CD%\cc65\cl65.exe" set "CC65_BIN=%CD%\cc65"
if exist "C:\cc65\bin\cl65.exe" set "CC65_BIN=C:\cc65\bin"
if exist "C:\cc65\cl65.exe" set "CC65_BIN=C:\cc65"
if not defined CC65_BIN (
  echo [ERROR] No se encontró cl65.exe. Edita este script o instala cc65 en una ruta reconocida.
  goto :end
)
REM Mostrar ruta detectada
if exist "%CC65_BIN%\cl65.exe" (
  echo [INFO] cl65.exe detectado en: %CC65_BIN%
) else (
  echo [ERROR] No se encontró cl65.exe en la ruta detectada: %CC65_BIN%
  goto :end
)
set "PATH=%CC65_BIN%;%PATH%"

REM === Compilación principal ===
make -f Makefile.nes
if %ERRORLEVEL% neq 0 (
  echo [ERROR] Error de compilación.
  goto :end
)
echo [OK] Compilación exitosa. Archivo generado: robloy_nes.nes

goto :end

:clean
make -f Makefile.nes clean
if %ERRORLEVEL% neq 0 (
  echo [ERROR] Error al limpiar archivos temporales.
  goto :end
)
echo [OK] Limpieza completada.
goto :end

:help
echo Uso: build_nes.bat [clean]
echo   Sin argumentos: compila el proyecto NES.
echo   clean: elimina archivos temporales de compilacion.
echo   /h, /?: muestra esta ayuda.
goto :end

:end
endlocal
pause