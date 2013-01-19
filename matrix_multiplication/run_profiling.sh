#!/bin/bash

NAME1="block_block-1"
NAME2="block_block-2"
mkdir -p results
rm results/${NAME1}
rm results/${NAME2}

echo "Compiling..."
make

echo "Running..."
for i in `seq 0 27`;
  do
    echo $i
    ./matrix_multiptication.out $i 1 "results/${NAME1}"
  done

for i in `seq 0 27`;
  do
    echo $i
    ./matrix_multiptication.out $i 2 "results/${NAME2}"
  done

echo "Done!"
