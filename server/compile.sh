#!/bin/bash

cd net
cd communication
gcc -c *.c
cd ..
gcc -c *.c
cd io
gcc -c *.c
cd ..
cd ..

cd game
cd login
gcc -c *.c
cd ..
cd map
gcc -c *.c
cd ..
cd entity
cd player
gcc -c *.c
cd .. 
cd ..
gcc -c *.c
cd ..

cd util
gcc -c *.c
cd ..

gcc -c *.c
gcc -pthread -o main *.c net/*.c net/io/*.c net/communication/*.c game/*.c game/login/*.c game/entity/player/*.c game/map/*.c util/*.c
