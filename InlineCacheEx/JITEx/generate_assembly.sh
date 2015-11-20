
# Compile
rm -r tmp
mkdir tmp
cp src/*.c tmp
cp src/*.h tmp
cp src/*.s tmp
cd tmp
gcc -g -O3 -std=c99 -S cfeeny.c utils.c ast.c bytecode.c vm.c vm.s compiler.c -Wno-int-to-void-pointer-cast || exit 1
