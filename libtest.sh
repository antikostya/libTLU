#!/bin/bash

if [[ $1 = 'libmem' ]]; then
	NAME=LIBMEM
	TARGET=./build/libmem_unittest.elf
	DIR=libc/native/mem
elif [[ $1 == 'libstring' ]]; then
	NAME=LIBSTRING
	TARGET=./build/libstring_unittest.elf
	DIR=libc/native/string
elif [[ $1 == 'libchar' ]]; then
	NAME=LIBCHAR
	TARGET=./build/libchar_unittest.elf
	DIR=libc/native/char
elif [[ $1 == 'cvector' ]]; then
	NAME=CVECTOR
	TARGET=./build/cvector_unittest.elf
	DIR=container/cvector
else
	echo 'evaliable tests: libmem, libstring, cvector'
	exit 1
fi

echo BUILDING $NAME FOR VALGRIND
CONFIG_SILENT=1 make --environment-overrides --quiet clean
CONFIG_SILENT=1 make --environment-overrides clean
CONFIG_SANITIZE=0 make --environment-overrides --quiet -j $TARGET
echo
echo VALGRIND $NAME
valgrind --leak-check=full -s $TARGET


echo
echo BILDING $NAME FOR GCOV
CONFIG_SILENT=1 make --environment-overrides --quiet clean
CONFIG_SANITIZE=0 make --environment-overrides --quiet -j $TARGET
echo
$TARGET
echo
for f in $(ls ${DIR}/*.c); do
	printf "GCOV $(basename $f):\t";
	gcov $f | tail -n1 | grep -oP "\d+[.]\d+[%]";
done

