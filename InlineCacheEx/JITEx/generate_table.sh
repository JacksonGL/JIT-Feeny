#!/bin/bash

function print_equal_space(){
echo -n "$((echo -n "$1" ; echo -n "                              ") | cut -c 1-15)"
}

for name in {bsearch,inheritance,cplx,lists,vector,fibonacci,sudoku,sudoku2,hanoi_BM,stacks_BM,morehanoi_BM}
do
	tname="$(echo -n $name | sed 's/_/\\_/g')"
	print_equal_space "$tname"
	for value in `cut -d ':' -f 2 < output/stat/"$name".feeny.stat | awk '{print $1 }' | sed 's/\(^[0-9]\+\.[0-9]\{3\}\)[0-9]*$/\1/g'`
	do
		echo -n "& "
		print_equal_space $value
	done
	echo '\\'
done
