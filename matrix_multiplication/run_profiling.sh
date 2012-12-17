#!/bin/bash

mkdir -p results
rm results/*

echo "Compiling..."
make

echo "Running..."
for i in `seq 0 27`;
  do
    echo $i
    ./matrix_multiptication.out $i "results/unrolled_block_block"
  done

echo "Done!"
