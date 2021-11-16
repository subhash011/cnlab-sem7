#!/bin/bash

# the input file
IN_FILE="arrivals.txt"

# number of packets that the input file contains
IN_PACKETS_COUNT=`cat $IN_FILE | wc -l`

# precision of the output (upto 6 decimal places in this case)
PRECISION_PLACES=6

# the predefined constats as per the question
TOKEN_BUCKET_CAPACITY="500"
FIFO_CAPACITY="1000"
FIFO_RATE="10.0"

# check if packets are lost when the traffic is passed
# through the token bucket followed by the FIFO queue.
function check_if_packets_lost
{
    # the rate of the token bucket
    rate=$1
    # run the traffic through the token bucket
    shape=`./shape $TOKEN_BUCKET_CAPACITY $rate < $IN_FILE > tempOut`
    # run the traffic through the FIFO queue
    fifo=`./fifo $FIFO_CAPACITY $FIFO_RATE < tempOut | wc -l`
    rm tempOut
    # check if the number of packets after the FIFO queue is
    # same as the number of packets in the actual traffic.
    if [ $fifo -eq $IN_PACKETS_COUNT ]; then
        echo 0 # no packets lost
    else
        echo 1 # packets are lost
    fi
}

# performs a binary search to find the largest rate
# at which packets are not lost.
function binary_search
{
    # the lower bound of the search
    low=0.0
    # the upper bound of the search
    high=$1
    # stores the previous mid value, this is used
    # because we have a fixed precision and we need
    # to break on convergence since the rate is a real value.
    prev_mid=0
    while [ $(echo "$low < $high" | bc -l) ]; do
        # find the mid value with the required precision.
        mid=$(echo "scale=$PRECISION_PLACES; ($low + $high) / 2" | bc)
        # if mid value is same as the previous mid value, we
        # have converged.
        if [ $(echo "$mid == $prev_mid" | bc -l) -eq 1 ]; then
            break
        fi
        prev_mid=$mid
        if [ `check_if_packets_lost $mid` -eq 0 ]; then
            # if packets are not lost, then we have to search
            # between mid and high.
            low=$mid
        else
            # if the packets are lost, then we have to search
            # between low and mid.
            high=$mid
        fi
    done
    # return mid value to which we converged.
    echo $mid
}

# a linear search algorithm for finding the largest rate
# at which packets are not lost.
function linear_search
{
    # initial rate
    counter=0.0
    rate=0
    while true;
    do
        if [ `check_if_packets_lost $counter` -eq 0 ]; then
            # if packets are not lost, then we have to search further.
            rate=$counter
        else
            # if the packets are lost, then we can stop, here previous
            # rate is the largest rate at which packets are not lost.
            break
        fi
        # increment the counter with the required precision.
        counter=$(echo "scale=$PRECISION_PLACES; $counter + 10^-$PRECISION_PLACES" | bc -l)
    done
    echo $rate
}

if [ ! -f ./shape ]; then
    echo "File not found! The binary file shape was not found."
elif [ ! -f ./fifo ]; then
    echo "File not found! The binary file fifo was not found."
else
    # largest_rate=$(linear_search)
    largest_rate=$(binary_search 10)
    echo "The largest rate at which packets are not lost is: $largest_rate"
fi