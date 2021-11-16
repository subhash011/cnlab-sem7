#!/bin/bash
args=("0.1 1 2 1 2" "1 2 1 2 1" "1 2 1 3 4" "10 1 1 2 2")

rm -f wfq_output.txt
touch wfq_output.txt
for arg in "${args[@]}"
do
    ./wfq $arg < arrivals.txt >> wfq_output.txt
done