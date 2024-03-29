
Print the bytecode
------------------

This is a utility to show the bytecode of a specified Feeny program
compiled by the standard Feeny bytecode compiler. To show the byte code
of Feeny code ```test/hello.feeny```, type the following command:

```
./print_bc.sh hello
```

The result will be written in ```output/hello.bc.std```.

Example of Byte Code Structure
------------------------------

```
Constants :
   #0: Null
   #1: String("real")
   #2: Slot(#1)
   #3: String("imag")
   #4: Slot(#3)
   #5: String("add")
   #6: String("cplx")
   #7: Method(#5, nargs:2, nlocals:0) :
         get local 0
         slot #1
         get local 1
         slot #1
         call-slot #5 2
         get local 0
         slot #3
         get local 1
         slot #3
         call-slot #5 2
         call #6 2
         return
   #8: String("sub")
   #9: Method(#8, nargs:2, nlocals:0) :
         get local 0
         slot #1
         get local 1
         slot #1
         call-slot #8 2
         get local 0
         slot #3
         get local 1
         slot #3
         call-slot #8 2
         call #6 2
         return
   #10: String("mul")
   #11: Method(#10, nargs:2, nlocals:0) :
         get local 0
         slot #1
         get local 1
         slot #1
         call-slot #10 2
         get local 0
         slot #3
         get local 1
         slot #3
         call-slot #10 2
         call-slot #8 2
         get local 0
         slot #1
         get local 1
         slot #3
         call-slot #10 2
         get local 0
         slot #3
         get local 1
         slot #1
         call-slot #10 2
         call-slot #5 2
         call #6 2
         return
   #12: String("div")
   #13: Method(#12, nargs:2, nlocals:1) :
         get local 1
         slot #1
         get local 1
         slot #1
         call-slot #10 2
         get local 1
         slot #3
         get local 1
         slot #3
         call-slot #10 2
         call-slot #5 2
         set local 2
         drop
         get local 0
         slot #1
         get local 1
         slot #1
         call-slot #10 2
         get local 0
         slot #3
         get local 1
         slot #3
         call-slot #10 2
         call-slot #5 2
         get local 2
         call-slot #12 2
         get local 0
         slot #3
         get local 1
         slot #1
         call-slot #10 2
         get local 0
         slot #1
         get local 1
         slot #3
         call-slot #10 2
         call-slot #8 2
         get local 2
         call-slot #12 2
         call #6 2
         return
   #14: String("conseq38")
   #15: String("end39")
   #16: Int(0)
   #17: String("lt")
   #18: String("conseq40")
   #19: String("end41")
   #20: String("eq")
   #21: String("~ + ~i")
   #22: String("~")
   #23: String("~ - ~i")
   #24: String("print")
   #25: Method(#24, nargs:1, nlocals:0) :
         get local 0
         slot #3
         lit #16
         call-slot #17 2
         branch #14
         get local 0
         slot #3
         lit #16
         call-slot #20 2
         branch #18
         get local 0
         slot #1
         get local 0
         slot #3
         printf #21 2
         goto #19
      label #18
         get local 0
         slot #1
         printf #22 1
      label #19
         goto #15
      label #14
         get local 0
         slot #1
         lit #16
         get local 0
         slot #3
         call-slot #8 2
         printf #23 2
      label #15
         return
   #26: Class(#2, #4, #7, #9, #11, #13, #25)
   #27: Method(#6, nargs:2, nlocals:0) :
         lit #0
         get local 0
         get local 1
         object #26
         return
   #28: Int(2)
   #29: Int(5)
   #30: String("a = ")
   #31: String("\n")
   #32: Int(-4)
   #33: String("b = ")
   #34: String("a + b = ")
   #35: String("a - b = ")
   #36: String("a * b = ")
   #37: String("a / b = ")
   #38: String("main")
   #39: Method(#38, nargs:0, nlocals:2) :
         lit #28
         lit #29
         call #6 2
         set local 0
         drop
         printf #30 0
         drop
         get local 0
         call-slot #24 1
         drop
         printf #31 0
         drop
         lit #32
         lit #28
         call #6 2
         set local 1
         drop
         printf #33 0
         drop
         get local 1
         call-slot #24 1
         drop
         printf #31 0
         drop
         printf #34 0
         drop
         get local 0
         get local 1
         call-slot #5 2
         call-slot #24 1
         drop
         printf #31 0
         drop
         printf #35 0
         drop
         get local 0
         get local 1
         call-slot #8 2
         call-slot #24 1
         drop
         printf #31 0
         drop
         printf #36 0
         drop
         get local 0
         get local 1
         call-slot #10 2
         call-slot #24 1
         drop
         printf #31 0
         drop
         printf #37 0
         drop
         get local 0
         get local 1
         call-slot #12 2
         call-slot #24 1
         drop
         printf #31 0
         return
   #40: String("entry42")
   #41: Method(#40, nargs:0, nlocals:0) :
         call #38 0
         drop
         lit #0
         return
Globals :
   #27
   #39
Entry : #41
```