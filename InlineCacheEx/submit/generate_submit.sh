#!/bin/bash

cp ../tests/hanoi_BM.feeny hanoi_LGBM.feeny
cp ../tests/stacks_BM.feeny stacks_LGBM.feeny
cp ../tests/morehanoi_BM.feeny morehanoi_LGBM.feeny

cp ../reports/report_LGBM.pdf .
cp ../src/compiler.c compiler_LGBM.c
cp ../src/vm.c vm_LGBM.c
cp ../src/vm.s vm_LGBM.s

zip -r assign7_LGBM.zip *.pdf *.c *.s *.feeny
