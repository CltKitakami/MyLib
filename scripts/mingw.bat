@echo off

REM set MINGW=D:\portable\MinGW\4.8.1-x86\bin
set MINGW=D:\portable\MinGW\4.8.1-x64\bin
set CXX=%MINGW%\g++
set CC=%MINGW%\gcc
set MAKE=%MINGW%\make
set GDB=%MINGW%\gdb
set NM=%MINGW%\nm
set OBJDUMP=%MINGW%\objdump
set STRIP=%MINGW%\strip
set AR=%MINGW%\ar
set PATH=%MINGW%

set CMAKE=D:\portable\MinGW\cmake-2.8.12.2-win32-x86\bin\cmake.exe
set CMAKE_GUI=D:\portable\MinGW\cmake-2.8.12.2-win32-x86\bin\cmake-gui.exe

set BUILDDIR=%~dp0..\build

exit /b 0
