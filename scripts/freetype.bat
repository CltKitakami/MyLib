@echo off

REM E:\freetype.exe E:\times.ttf abcdefghijklmnopqrstuvwxyz0123456789

set VCPATH=C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC
set WINSDKPATH=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A

set PATH=%PATH%;%VCPATH%\bin
REM set PATH=%PATH%;%VCPATH%\VSTSDB\Deploy
set PATH=%PATH%;%VCPATH%\..\Common7\IDE
REM set PATH=%PATH%;%VCPATH%\..\Common7\Tools
REM set PATH=%PATH%;%VCPATH%\VCPackages
REM set PATH=%PATH%;%WINSDKPATH%\bin

set INCLUDE=%INCLUDE%;%VCPATH%\include
REM set INCLUDE=%INCLUDE%;%VCPATH%\atlmfc\include

set LIB=%LIB%;%VCPATH%\lib
set LIB=%LIB%;%VCPATH%\atlmfc\lib
set LIB=%LIB%;%WINSDKPATH%\lib

set LIBPATH=%LIBPATH%;%VCPATH%\lib
set LIBPATH=%LIBPATH%;%VCPATH%\atlmfc\lib
REM set LIBPATH=%LIBPATH%;C:\Windows\Microsoft.NET\Framework\v4.0.30319
REM set LIBPATH=%LIBPATH%;C:\Windows\Microsoft.NET\Framework\v3.5

REM call "%VCPATH%\vcvarsall.bat"
REM echo %PATH%
REM echo %INCLUDE%
REM echo %LIB%
REM echo %LIBPATH%


set userPath=%rootPath%\..\MinGW64
set freetypeDir=%userPath%\freetype_2_3_5_1
set freetypeInc=/I "%freetypeDir%\include"
set freetypeLib=/LIBPATH:"%freetypeDir%\lib" freetype.lib
set cxxflags=%freetypeInc% /nologo
set ldflags=-Wl,-static %cpu% %sysLibPath% -lwsock32 -lpsapi %opencvLib% %openalLib% %freetypeLib%

"%VCPATH%\bin\cl.exe" /c E:\freetype.c /Fo"E:\\" /nologo /Wall /D _WIN32 E:\freetype.c %freetypeInc%

"%VCPATH%\bin\link.exe" E:\freetype.obj /nologo "%freetypeDir%\lib\freetype.lib" /OUT:E:\freetype.exe