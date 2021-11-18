/**
 * This program simulates the FIFO queue packet scheduling algorithm.
 * It takes two command line arguments:
 *      1. The maximum number of bytes the queue can hold.
 *      2. The at which the queue spits packets.
 * It then reads in a sequence of (X, Y, Z) from stdin where X is the
 * arrival time of a packet, Y is the packet identifier, and Z is the
 * size of the packet.
 * Sample usage:
 *      1. Compile: gcc -o fifo fifo.c
 *      2. Run: ./fifo  500 1 < arrivals.txt (arrivals.txt contains the packet sequence)
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct Packet
{
    int packet_id;
    int packet_size;
    float arrival_time;
    float service_time; // time when the packet is sent into the network (removed from the FIFO queue).
    struct Packet *next;
} Packet;

// A deque implementation using a linked list
typedef struct Queue
{
    Packet *front;
    Packet *back;
    int size; // current size of the queue in bytes.
} Queue;

Queue *createQueue()
{
    // Allocate memory for the queue
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->front = NULL;
    queue->back = NULL;
    // Initialize the size of the queue
    queue->size = 0;
    return queue;
}

Packet *createPacket(int packet_id, int packet_size, float arrival_time, float service_time)
{
    // Allocate memory for the packet
    Packet *packet = (Packet *)malloc(sizeof(Packet));
    packet->packet_id = packet_id;
    packet->packet_size = packet_size;
    packet->arrival_time = arrival_time;
    packet->service_time = service_time;
    return packet;
}

void push(Queue *queue, Packet *node)
{
    // If the queue is empty, the node is the front and the back
    if (queue->front == NULL)
    {
        queue->front = node;
        queue->back = node;
        // Increase the size of the queue by the size of the packet
        queue->size += node->packet_size;
        return;
    }
    // Else, add the node to the back of the queue and update the size
    queue->size += node->packet_size;
    queue->back->next = node;
    queue->back = node;
}

Packet *peek_back(Queue *queue)
{
    // Return the last packet in the queue (the one that came in latest)
    return queue->back;
}

Packet *peek_front(Queue *queue)
{
    // Return the first packet in the queue (the one that came in earliest)
    return queue->front;
}

Packet *pop(Queue *queue)
{
    // If the queue is empty, return NULL
    if (queue->front == NULL)
    {
        return NULL;
    }
    // Else, remove the first packet in the queue and update the size
    Packet *node = queue->front;
    queue->front = node->next;
    queue->size -= node->packet_size;
    // If the queue is empty, update the back pointer to NULL
    if (queue->size == 0)
    {
        queue->back = NULL;
    }
    return node;
}

// simulates the FIFO queue algorithms.
void simulate_fifo_queue(int queue_capacity, float output_rate)
{
    int packet_id, packet_length;
    float arrival_time;
    Queue *queue = createQueue();
    Packet *front, *back;
    while (scanf("%f %d %d", &arrival_time, &packet_id, &packet_length) != EOF)
    {
        // If packet size is larger than the queue, it can never enter the queue.
        if (packet_length > queue_capacity)
        {
            continue;
        }
        // Remove the packet from the queue if it is has already been sent into the network.
        while ((front = peek_front(queue)) && front->service_time <= arrival_time)
        {
            // print the necessary details.
            printf("%.2f %d %d\n", front->service_time, front->packet_id, front->packet_size);
            pop(queue);
        }
        // If there is enough space in the queue, add the packet to the queue.
        if (queue->size + packet_length <= queue_capacity)
        {
            back = peek_back(queue);
            float time = packet_length / output_rate;
            float service_time = 0;
            if (back)
            {
                // if there is a packet in the queue, the current packet will
                // be sent into the network at the FIFO rate after the previous
                // packet has been sent.
                service_time = back->service_time + time;
            }
            else
            {
                // if there is no packet in the queue, the current packet will
                // be sent into the network at the FIFO rate immediately.
                service_time = arrival_time + time;
            }
            // Create a new packet and add it to the queue.
            Packet *packet = createPacket(packet_id, packet_length, arrival_time, service_time);
            push(queue, packet);
        }
    }
    // Remove the remaining packets from the queue.
    while ((front = peek_front(queue)))
    {
        // print the necessary details.
        printf("%.2f %d %d\n", front->service_time, front->packet_id, front->packet_size);
        pop(queue);
    }
}

int main(int argc, char *argv[])
{
    int queue_capacity = atoi(argv[1]);
    float output_rate = atof(argv[2]);
    simulate_fifo_queue(queue_capacity, output_rate);
    return 0;
}