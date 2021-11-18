/**
 * This program simulates the token bucket traffic shaping algorithm.
 * It takes two command line arguments:
 *      1. The maximum number of tokens the bucket can hold.
 *      2. The rate of tokens per second
 * It then reads in a sequence of (X, Y, Z) from stdin where X is the
 * arrival time of a packet, Y is the packet identifier, and Z is the
 * size of the packet.
 * Sample usage:
 *      1. Compile: gcc -o shape shape.c
 *      2. Run: ./shape 500 1 < arrivals.txt (arrivals.txt contains the packet sequence)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// A macro to find the minimum of two numbers
#define min(a, b) ((a) < (b) ? (a) : (b))

// A structure to represent the bucket.
typedef struct TokenBucket
{
    int bucket_size;
    int tokens_in_bucket; // Number of tokens in the bucket at any instant in time.
    float fill_rate;      // The rate at which tokens are added to the bucket.
    float last_send_time; // The previous time step when tokens were sent.
} TokenBucket;

TokenBucket *token_bucket;

void initialize_token_bucket(int bucket_size, float fill_rate)
{
    token_bucket = (TokenBucket *)malloc(sizeof(TokenBucket));
    token_bucket->bucket_size = bucket_size;
    token_bucket->tokens_in_bucket = bucket_size;
    token_bucket->fill_rate = fill_rate;
    token_bucket->last_send_time = 0;
}

void add_tokens_to_bucket(int num_tokens_to_add)
{
    // add tokens to bucket
    token_bucket->tokens_in_bucket += num_tokens_to_add;
    // if there is overflow, discard additional tokens
    token_bucket->tokens_in_bucket = min(token_bucket->tokens_in_bucket, token_bucket->bucket_size);
}

/**
 * Tries to send a packet if there are enough tokens in the bucket.
 * Returns true if the packet was sent, false otherwise.
*/
bool try_to_send(int packet_length)
{
    // check if there are enough tokens in the bucket
    bool can_consume = token_bucket->tokens_in_bucket >= packet_length ? true : false;
    if (can_consume)
    {
        // consume tokens from the bucket
        token_bucket->tokens_in_bucket -= packet_length;
        return true;
    }
    return false;
}

/**
 * Simulates the token bucket traffic shaping algorithm.
*/
void simulate_token_bucket(int token_bucket_size, float token_bucket_fill_rate)
{
    int packet_id, packet_length;
    float arrival_time;
    // initialize the token bucket
    initialize_token_bucket(token_bucket_size, token_bucket_fill_rate);
    while (scanf("%f %d %d", &arrival_time, &packet_id, &packet_length) != EOF)
    {
        /**
         * if no other packet is waiting to be sent,
         * add tokens to the bucket and set the last_send_time
         * to the current time (arrival_time).
        */
        if (arrival_time > token_bucket->last_send_time)
        {
            int num_tokens_to_add = token_bucket->fill_rate * (arrival_time - token_bucket->last_send_time);
            add_tokens_to_bucket(num_tokens_to_add);
            token_bucket->last_send_time = arrival_time;
        }
        // if the bucket does not have enough tokens to send the packet
        if (!try_to_send(packet_length))
        {
            // set last_send_time to when the packet will be sent and empty the bucket.
            token_bucket->last_send_time += ((packet_length - token_bucket->tokens_in_bucket) / token_bucket->fill_rate);
            token_bucket->tokens_in_bucket = 0;
        }
        printf("%.2f %d %d\n", token_bucket->last_send_time, packet_id, packet_length);
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <token_bucket_size> <token_bucket_fill_rate>\n", argv[0]);
        exit(0);
    }
    int token_bucket_size = atoi(argv[1]);
    float token_bucket_fill_rate = atof(argv[2]);
    simulate_token_bucket(token_bucket_size, token_bucket_fill_rate);
    return 0;
}
