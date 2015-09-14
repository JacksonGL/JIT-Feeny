
Test Harness
------------

The provided bash script ```run_tests``` will compile the test programs in the ```test``` directory into bytecodes and run your bytecode interpreter on the result. To run it, type:
```
./run_tests vm.c
```

Compiling and Running Manually
------------------------------

To compile and run your implementation of the bytecode interpreter manually, you may also follow these steps. Compile your interpreter by typing:

```
gcc src/cfeeny.c src/utils.c src/bytecode.c src/vm.c -o cfeeny -Wno-int-to-void-pointer-cast
```

at the terminal. This will create the ```cfeeny``` executable.

Next, you have to run the supplied compiler which will read in the source text for a Feeny program and compile it to a binary file containing the bytecode instructions.

```
./compiler -i tests/cplx.feeny -o output/cplx.bc
```

Finally, call the ```cfeeny``` executable with the binary bytecode file as its argument to start the interpreter.

```
./cfeeny output/cplx.bc
```