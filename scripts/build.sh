#!/bin/bash

# sudo apt-get install libssl-dev libncurses5-dev zlib1g-dev

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

