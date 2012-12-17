#!/bin/bash

NAME="simple"
mkdir -p results
rm results/${NAME}

echo "Compiling..."
make

echo "Running..."
for i in `seq 0 27`;
  do
    echo $i
    ./matrix_multiptication.out $i "results/${NAME}"
  done

echo "Done!"
