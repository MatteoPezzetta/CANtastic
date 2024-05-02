#!/bin/bash
gcc src/can_write.c -o can_write
g++ src/main.cpp -I inc -o main -pthread
