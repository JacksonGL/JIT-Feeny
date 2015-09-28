#!/bin/bash
# this is a util tool for printing
# the byte code compiled from the
# standard byte code compiler

rm output/*.ast
rm output/*.out
rm output/*.bc
rm output/*.bc.txt

# compile the byte code printer
gcc -O3 -std=c99 src/bc_printer.c src/utils.c src/ast.c src/bytecode.c -o bc_printer -Wno-int-to-void-pointer-cast

# first compile the feeny code into
# byte code
if [ "$(uname)" == "Darwin" ]; then
	# Do something under Mac OS X platform
	./compiler_osx -i tests/$1.feeny -o output/$1.bc
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	# Do something under Linux platform
	./compiler -i tests/$1.feeny -o output/$1.bc
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
	# Do something under Windows NT platform
	echo 'OS not supported. Abort.'
fi

echo " "
echo " "

# run the byte code printer
./bc_printer output/$1.bc 2>&1 | tee output/$1.bc.txt

echo " "
echo " "
echo "The result is also written in "output/$1.bc.txt"."