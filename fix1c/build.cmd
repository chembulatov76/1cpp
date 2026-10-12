@echo off
rem Build OdbcFix1C.dll with VC6. Toolchain: C:\VS6 and the 1C SDK C:\Install\vc6\1c-sdk
rem (the same as Json1C, see C:\Projects\json-1c\docs\toolchain.md).
setlocal
if "%VS6%"=="" set VS6=C:\VS6
if "%SDK%"=="" set SDK=C:\Install\vc6\1c-sdk
set PATH=%VS6%\COMMON\MSDEV98\BIN;%VS6%\VC98\BIN;%PATH%
cd /d "%~dp0"
if not exist Out\gen mkdir Out\gen
python -B gen_names.py
if errorlevel 1 exit /b 1
rem always clean: nmake does not track header changes reliably here, and VC6 builds this in seconds
nmake /nologo /f Makefile VS6=%VS6% SDK=%SDK% clean >nul 2>&1
nmake /nologo /f Makefile VS6=%VS6% SDK=%SDK% %*
