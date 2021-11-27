#!/bin/bash

test() {
    python3 q1.py
    python3 q2.py
    python3 q3.py > temp.txt
    DIFF=$(echo | cat message.txt - | diff temp.txt -)
    rm temp.txt
    if [ "$DIFF" != "" ] 
    then
        echo "Doesn't match"
    else
        echo "Match"
    fi
}

test