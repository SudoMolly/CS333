#!/bin/bash
x="!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_\`abcdefghijklmnopqrstuvwxyz{|}~" 
echo $x
./cae-xor -d -c $x < allpossible.tst