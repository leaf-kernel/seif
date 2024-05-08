#!/bin/bash
python3 test.py
gcc -o main main.c
./main out.seif
rm main
