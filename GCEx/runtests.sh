# Compile
gcc -g -O3 -rdynamic -std=c99 src/cfeeny.c src/utils.c src/ast.c src/bytecode.c src/vm.c src/compiler.c -o cfeeny -Wno-int-to-void-pointer-cast || exit 1

if [ "$1" != "-nvg" ]
then
	RUN_VG=true
else
	RUN_VG=false
fi

# Clean output folder
mkdir -p output/reference
mkdir fail
mkdir output/stat
rm output/*.out
rm output/reference/*.out
rm output/*.bc
rm output/stat/*.stat
rm output/*

# Run output
function test {
	INPUT="$1"
	OUTPUT="$(basename "$INPUT")"
	echo "INPUT" "$INPUT"
	echo "OUTPUT" "$OUTPUT"
	if [ "$(uname)" == "Darwin" ]; then
    	# Do something under Mac OS X platform
    	# compile feeny program into AST
    	./parser_osx -i "$INPUT" -oast output/"$OUTPUT".ast
    	# compile feeny program into byte code
    	./compiler_osx -i "$INPUT" -o output/"$OUTPUT".bc
    	# collect oracle
		./feeny_osx -e "$INPUT" | tail -n +2 | sed -e '$ d'  > output/reference/"$OUTPUT".out
		[ "$(tail -n 1 output/reference/"$OUTPUT".out | grep -E '^ERROR:')" == "" ]
		PASSED="$?"
	elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    	# Do something under Linux platform
    	# compile feeny program into AST
    	./parser -i "$INPUT" -oast output/"$OUTPUT".ast
    	# compile feeny program into byte code
    	./compiler -i "$INPUT" -o output/"$OUTPUT".bc
    	# collect oracle
		./feeny -e "$INPUT" | tail -n +2 > output/reference/"$OUTPUT".out
		[ "$(tail -n 1 output/reference/"$OUTPUT".out | grep -E '^ERROR:')" == "" ]
		PASSED="$?"
	elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
    	# Do something under Windows NT platform
		echo 'Huh?'
	fi
	if which valgrind && $RUN_VG
	then
		valgrind --log-file="valgrind.out" --error-exitcode=121 \
			./cfeeny output/"$OUTPUT".ast output/stat/"$OUTPUT".stat > output/"$OUTPUT".out
		RESULT="$?"
		if [ "$RESULT" == "121" ]
		then
			echo "Valgrind error!"
			diff output/"$OUTPUT".out output/reference/"$OUTPUT".out
			cp "$INPUT" fail/
			exit 1
		fi
		echo "Valgrind result: $RESULT"

		[ "$RESULT" == "0" ] || [ "$RESULT" == "139" ] # segv = 139
		US="$?"
	else
		# ./cfeeny output/"$OUTPUT".bc -s output/stat/"$OUTPUT".stat > output/"$OUTPUT".out
		./cfeeny output/"$OUTPUT".ast output/stat/"$OUTPUT".stat > output/"$OUTPUT".out
		[ "$?" == "0" ]
		US="$?"
	fi
	if [ "$US" != "$PASSED" ]
	then
		echo "DIFFERING RETURN CODES!"
		echo "US: $US"
		echo "REFERENCE: $PASSED"
		diff output/"$OUTPUT".out output/reference/"$OUTPUT".out
		cp "$INPUT" fail/
		exit 1
	fi

	if [ "$PASSED" != "0" ]
	then
		return
	fi

	diff output/"$OUTPUT".out output/reference/"$OUTPUT".out
	if [ "$?" != "0" ]
	then
		cp "$INPUT" fail/
		exit 1
	fi

	# if we reach here, we have "passed the test case"
	if [ "$(dirname "$INPUT")" == "fail" ]
	then
		rm "$INPUT"
	fi

}

#test "tests/sudoku.feeny"

for a in `find fail/ -maxdepth 1 -type f -name "*.feeny" -exec wc -l {} \; | sort -n | awk '{ print $2 }'`
do
	test "$a"
done

for a in `find tests/fail_unit_tests -maxdepth 1 -type f -name "*.feeny" -exec wc -l {} \; | sort -n | awk '{ print $2 }'`
do
	test "$a"
done

for a in `find tests/unit_test/ -maxdepth 1 -type f -name "*.feeny" -exec wc -l {} \; | sort -n | awk '{ print $2 }'`
do
	test "$a"
done

for a in `find tests/ -maxdepth 1 -type f -name "*.feeny" -exec wc -l {} \; | sort -n | awk '{ print $2 }'`
do
	test "$a"
done
