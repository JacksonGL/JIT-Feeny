#!/bin/bash

# Compile
gcc -std=c99 -O3 src/cfeeny.c src/utils.c src/ast.c src/objects.c src/interpreter.c -o cfeeny -Wno-int-to-void-pointer-cast

# Clean output folder
rm output/*.out
rm output/*.ast
rm output/reference/*.out

function all_but_last () {
  while read A
  do
    if [ ! -z "$LAST" ]
    then
      echo $LAST
    fi
    LAST="$A"
  done
  unset LAST
}

# Run output
function test {
  if [ "$(uname)" == "Darwin" ]; then
      # Do something under Mac OS X platform    
      ./parser_osx -i tests/unit_test/$1.feeny -oast output/$1.ast
      ./osx_feeny -e tests/unit_test/$1.feeny | (read A ; cat /dev/stdin ) | all_but_last > output/reference/$1.out
  elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
      # Do something under Linux platform
      ./parser -i tests/unit_test/$1.feeny -oast output/$1.ast
      ./feeny -e tests/unit_test/$1.feeny > output/reference/$1.out   
  elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
      # Do something under Windows NT platform
      echo "current build does not support windows"
      exit
  fi

  ./cfeeny output/$1.ast > output/$1.out
  if diff output/$1.out output/reference/$1.out >/dev/null ; then
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
