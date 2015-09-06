#!/bin/bash

# Compile
gcc -std=c99 -O3 src/cfeeny.c src/utils.c src/ast.c src/interpreter.c -o cfeeny -Wno-int-to-void-pointer-cast

# Clean output folder
rm output/*.out
rm output/*.ast

# Run output
function test {
  ./parser_osx -i tests/unit_test/$1.feeny -oast output/$1.ast
  ./cfeeny output/$1.ast > output/$1.out
  if [[ `cat output/$1.out` == `cat tests/unit_test/$1.oracle` ]] 
    then
    echo $1": pass"
  else
    echo $1": fail"
  fi
}

for file in tests/unit_test/*.feeny; do
	# remove the .feeny suffix
	# regular expression: tests/unit_test/(.*).feeny
  file=`echo $file | sed 's/tests\/unit_test\/\(.*\)[.]feeny$/\1/'`
  test $file
done
