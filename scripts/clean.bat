@echo off

call mingw.bat

cd %BUILDDIR%

"%MAKE%" clean

exit /b 0
