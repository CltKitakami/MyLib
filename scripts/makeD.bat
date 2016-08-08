@echo off

call mingw.bat

if not exist %BUILDDIR%D mkdir %BUILDDIR%D
cd %BUILDDIR%D

"%CMAKE%" -G "MinGW Makefiles" .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_PLATFORM=x64

if not exist %BUILDDIR%D exit /b 1

cd %BUILDDIR%D

"%MAKE%" -j 4

exit /b 0
