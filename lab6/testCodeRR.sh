#!/bin/bash
args=("0.1" "1" "10" "100")

rm -f rr_output.txt
touch rr_output.txt
for arg in "${args[@]}"
do
    ./rr $arg < arrivals.txt >> rr_output.txt
done