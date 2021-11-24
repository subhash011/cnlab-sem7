#!/bin/bash

python3 q1.py > /dev/null
python3 q2.py
printf "%s" "$(python3 q3.py)" > output.txt

DIFF=$(diff message.txt output.txt) 
if [ "$DIFF" != "" ] 
then
    echo "Doesn't match"
else
    echo "Match"
fi