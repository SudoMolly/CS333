#!/bin/bash

cd ..
make debug
cd client

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --leak-resolution=high --read-inline-info=yes --read-var-info=yes  --xtree-memory=full --show-reachable=yes -s $1 $2 $3 $4