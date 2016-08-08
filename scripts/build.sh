#!/bin/bash

cd ..
mkdir -p buildLinux
cd buildLinux
cmake -DCMAKE_BUILD_TYPE=Release -DEXT_INCS="" -DEXT_LIBS="-lssl -lcrypto" ..

if [ "$?" == "0" ]; then
	make
	exit 0
else
	echo cmake exit $?
	exit 1
fi

