@echo off

call mingw.bat

if not exist %BUILDDIR% exit /b 1

cd %BUILDDIR%

"%MAKE%" -j 4

exit /b 0
