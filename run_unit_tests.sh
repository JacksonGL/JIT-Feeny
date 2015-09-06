#!/bin/bash

# Compile
gcc -std=c99 -O3 src/cfeeny.c src/utils.c src/ast.c src/objects.c src/interpreter.c -o cfeeny -Wno-int-to-void-pointer-cast

# Clean output folder
rm output/*.out
rm output/*.ast

# Run output
function test {
  if [ "$(uname)" == "Darwin" ]; then
      # Do something under Mac OS X platform    
      ./parser_osx -i tests/unit_test/$1.feeny -oast output/$1.ast    
  elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
      # Do something under Linux platform
      ./parser -i tests/unit_test/$1.feeny -oast output/$1.ast   
  elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
      # Do something under Windows NT platform
      echo "current build does not support windows"
      exit
  fi

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
