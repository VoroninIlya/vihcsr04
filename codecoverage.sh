#!/bin/bash

lcov -t "tst_vihcsr04 -o tst_vihcsr04.info -c -d .
genhtml -o report tst_vihcsr04.info