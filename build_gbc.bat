@echo off
REM Compila Robloy GBC automáticamente usando GBDK-2020
setlocal
if not defined GBDK_HOME (
  set "GBDK_HOME=C:\gbdk-win64\gbdk"
)
set PATH=%GBDK_HOME%\bin;%PATH%
make -f Makefile.gbc
if %ERRORLEVEL% neq 0 (
  echo Error de compilación
)
echo Compilación exitosa. Archivo generado: robloy_gbc.gb
