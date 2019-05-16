#!/bin/bash

cd net
gcc -c *.c
cd io
gcc -c *.c
cd ..
cd ..

gcc -c *.c
gcc -pthread -o main *.c net/*.c net/io/*.c