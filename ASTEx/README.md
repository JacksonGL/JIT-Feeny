

Test Harness
------------

The provided bash script run_tests will compile, parse, and interpret the test programs in the test directory. To run it, type:
```
./run_tests interpreter.c
```

Compiling and Running Manually
------------------------------

To compile and run your implementation of the interpreter manually, you may also follow these steps. Compile your interpreter by typing:

```
gcc src/cfeeny.c src/utils.c src/ast.c src/interpreter.c -o cfeeny -Wno-int-to-void-pointer-cast
```

at the terminal. This will create the cfeeny executable.


Next, you have to run the supplied parser which will read in the source text for a Feeny program and dump it to a binary AST file.

```
./parser -i tests/cplx.feeny -oast output/cplx.ast
```

Finally, call the cfeeny executable with the binary AST file as its argument to start the interpreter.

```
./cfeeny output/cplx.ast
```

Running Unit Tests
------------------

To run unit tests:
```
./run_unit_tests.sh
```