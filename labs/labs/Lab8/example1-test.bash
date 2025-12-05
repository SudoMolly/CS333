#!/bin/bash
# Beginning sample script
#
# R Jesse Chaney
# rchaney@pdx.edu

make all

./example1

./example1 -n 5

gcov -c example1
