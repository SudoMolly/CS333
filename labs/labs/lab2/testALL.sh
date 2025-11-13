#!/bin/bash
clear
make debug 

echo "errors:"
echo "---------------"
./arvik-md4 -c 0-s.txt > output.me
echo "-------------"
echo "output:"
echo "-------------"
cat output.me