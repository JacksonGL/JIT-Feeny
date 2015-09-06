#!/bin/bash

gcc src/cfeeny.c src/utils.c src/ast.c src/interpreter.c -o cfeeny -Wno-int-to-void-pointer-cast

./parser_osx -i tests/unit_test/test1.feeny -oast output/cplx.ast

./cfeeny output/cplx.ast