@echo off

call mingw.bat

set FOLDER=bin
set EXE=main.exe

start "" "%GDB%" "%BUILDDIR%\%FOLDER%\%EXE%"

exit /b 0
