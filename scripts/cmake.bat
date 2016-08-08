@echo off

call mingw.bat

if not exist %BUILDDIR% mkdir %BUILDDIR%
cd %BUILDDIR%

set LIB_ROOT=D:/portable/MinGW

set ZLIB_INC=%LIB_ROOT%/zlib/1.2.8/include
set ZLIB_LIB=-L%LIB_ROOT%/zlib/1.2.8/x64/staticlib -lzlib

set ICONV_INC=%LIB_ROOT%/iconv/include
set ICONV_LIB=-L%LIB_ROOT%/iconv/x64/staticlib -liconv

set OPENSSL_INC=%LIB_ROOT%/openssl/include
set OPENSSL_LIB=-L%LIB_ROOT%/openssl/x64/staticlib -lssl -lcrypto -lgdi32

set EXT_INCS=%ZLIB_INC%;%ICONV_INC%;%OPENSSL_INC%
set EXT_LIBS=%ZLIB_LIB%;%ICONV_LIB%;%OPENSSL_LIB%

"%CMAKE%" -G "MinGW Makefiles" .. -DCMAKE_BUILD_TYPE=Release -DBUILD_PLATFORM=x64 -DEXT_INCS="%EXT_INCS%" -DEXT_LIBS="%EXT_LIBS%"

pause

exit /b 0
