#!/bin/bash
clear
make debug 

echo "errors:"
echo "---------------"
./arvik-md4 -c ./alphabet_test.txt > output.me
./TEST -c ./alphabet_test.txt > output.him
echo "-------------"
echo "output:"
echo "-------------"
cat output.me

echo "------------"
echo "diff: "
cmp -l output.me output.him