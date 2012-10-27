#!/bin/bash

mkdir -p instructions
mkdir -p cycles
rm instructions/*
rm cycles/*

echo "Compiling..."
make

echo "Running..."
for i in `seq 0 25`;
  do
    echo $i
    ./matrix_addition.out $i
    perf record -g -e instructions -o "./instructions/$i" ./matrix_addition.out $i >"stdout.log" 2>"stderr.log"
    perf report -n -i "./instructions/$i" >"./instructions/out_$i" 2>"stderr.log"
    perf record -g -e cycles -o "./cycles/$i" ./matrix_addition.out $i >"stdout.log" 2>"stderr.log"
    perf report -n -i "./cycles/$i" >"./cycles/out_$i" 2>"stderr.log"
  done

echo "Counting IPC..."
g++ -c get_info.cpp -std=c++0x
g++ -o get_info get_info.o
./get_info

echo "Done!"
