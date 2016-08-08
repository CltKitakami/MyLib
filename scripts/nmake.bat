@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"

set CMAKE=D:\portable\MinGW\cmake-2.8.12.2-win32-x86\bin\cmake.exe

set BUILDDIR="%~dp0..\buildMsvc"
if not exist %BUILDDIR% mkdir %BUILDDIR%
cd /d "%BUILDDIR%"

set LIB_ROOT=D:/portable/MinGW

set ZLIB_INC=%LIB_ROOT%/zlib/1.2.8/msvc/include
set ZLIB_LIB=%LIB_ROOT%/zlib/1.2.8/msvc/staticlib/zlibstatic.lib

set ICONV_INC=%LIB_ROOT%/iconv/include
set ICONV_LIB=%LIB_ROOT%/iconv/msvc/libiconvStatic.lib

set OPENSSL_INC=%LIB_ROOT%/openssl/include
set OPENSSL_LIB=%LIB_ROOT%/openssl/msvc/staticlib/ssleay32.lib
set OPENSSL_LIB=%OPENSSL_LIB%;%LIB_ROOT%/openssl/msvc/staticlib/libeay32.lib

set EXT_INCS=%ZLIB_INC%;%ICONV_INC%;%OPENSSL_INC%
set EXT_LIBS=%ZLIB_LIB%;%ICONV_LIB%;%OPENSSL_LIB%

"%CMAKE%" -G "NMake Makefiles" .. -DCMAKE_BUILD_TYPE=Release -DEXT_INCS="%EXT_INCS%" -DEXT_LIBS="%EXT_LIBS%"

nmake /F Makefile /nologo
