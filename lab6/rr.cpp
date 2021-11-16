#include <bits/stdc++.h>
#include <optional>
using namespace std;

inline int positive_modulo(int i, int n)
{
    return (i % n + n) % n;
}

class Packet
{
public:
    int id;
    int size;
    float arrival_time;
    int queue_id;

    Packet() : queue_id(-1) {}

    Packet(int id, int size, float arrival_time, int queue_id)
    {
        this->id = id;
        this->size = size;
        this->arrival_time = arrival_time;
        this->queue_id = queue_id;
    }
};

int cur_queue_id = -1;                   // stores the current queue from which a packet was popped.
set<int> queue_ids;                      // stores the list of queue ids in a sorted order.
unordered_map<int, list<Packet>> queues; // map of queue ids and their packets.
float transmission_time = 0.0;           // transmission time of the current packet.z

// add a packet to the queue
void add_packet(const Packet &packet)
{
    int queue_id = packet.queue_id;
    if (queue_ids.find(queue_id) == queue_ids.end())
        queue_ids.insert(queue_id);     // if the queue id is not in the set, add it.
    queues[queue_id].push_back(packet); // push the packet to the back of the queue.
}

// given a queue id, it returns the next queue in round-robin.
int next_queue(int queue_id)
{
    int cur_idx = distance(queue_ids.begin(), queue_ids.find(queue_id)); // get index of current queue id.
    int n_queues = queue_ids.size();
    int next_idx = (cur_idx + 1) % n_queues; // round-robin increment.
    auto it = queue_ids.begin();
    advance(it, next_idx); // from the start, move to next_idx
    return *it;
}

// given a queue id, it returns the next queue in round-robin.
int prev_queue(int queue_id)
{
    int cur_idx = distance(queue_ids.begin(), queue_ids.find(queue_id)); // get index of current queue id.
    int n_queues = queue_ids.size();
    int next_idx = positive_modulo(cur_idx - 1, n_queues); // round-robin increment.
    auto it = queue_ids.begin();
    advance(it, next_idx); // from the start, move to next_idx
    return *it;
}

// gets the next packet to be transmitted, this
// returns pair<empty packet, -1> if there are no
// packets to transmit.
Packet peek_packet()
{
    if (!queues.size())
        return Packet(); // no queues have been added yet.
    if (cur_queue_id == -1)
        // this is the first time a peek has been requested, so
        // initialise cur_queue_id to the last queue so that the next queue
        // returned is the first queue.
        cur_queue_id = *queue_ids.rbegin();
    int queue_id = next_queue(cur_queue_id); // get the next queu
    while (queues[queue_id].size() == 0)     // loop till the current queue is not empty.
    {
        if (queue_id == cur_queue_id) // finished one complete rotation
        {
            if (queues[queue_id].size() == 0) // if no packets in the initial queue, then all queues are empty
                return Packet();
            break; // if there is a packet, this is the next queue
        }
        queue_id = next_queue(queue_id);
    }
    // return the packet and queue
    return queues[queue_id].front();
}

void remove_packet(int queue_id)
{
    cur_queue_id = queue_id;                          // update the cur_queue_id since this is the queue from which we transmit.
    queues[queue_id].erase(queues[queue_id].begin()); // remove the packet from the front of the queue.
}

void transmit(const Packet &packet, float service_rate)
{
    // transmission starts either from the arrival time if the output line is free or
    // the time at which the output line gets freed.
    transmission_time = max(transmission_time, packet.arrival_time) + ((float)packet.size / service_rate);
    printf("%.2f %d\n", transmission_time, packet.id);
}

// send a packet over the output line.
void send_packets(float arrival_time, float service_rate)
{
    while (true)
    {
        Packet to_transmit = peek_packet(); // get the packet that has to be transmitted.
        int queue_id = to_transmit.queue_id;
        if (queue_id == -1)
            break;                           // if no packets are in the queues, stop sending
        remove_packet(queue_id);             // if there is a packet, remove it from the queue.
        transmit(to_transmit, service_rate); // transmit the packet on the output line.
        if (transmission_time >= arrival_time)
            break; // if the transmission time has exceeded the current arrival time, we break
    }
}

// this function splits a space delimited string into a vector.
void tokenize(string const &str, const char delim, vector<string> &out)
{
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}

// creates a packet from a line of input.
Packet make_packet(string s)
{
    vector<string> tokens;
    tokenize(s, ' ', tokens); // get the list of packet data
    int id = stoi(tokens[1]);
    int size = stoi(tokens[3]);
    float arrival_time = stof(tokens[0]);
    int queue_id = stoi(tokens[2]);
    return Packet(id, size, arrival_time, queue_id); // create packet and return it.
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./rr <servicerate> < <inputfile>\n");
        return 0;
    }
    float service_rate = stof(argv[1]);
    float arrival_time;
    string line;
    vector<string> packets;
    while (getline(cin, line))
        packets.push_back(line); // read the file as a list of strings.

    int i = 0;
    Packet packet;
    while (i < packets.size())
    {
        // create a packet from current line.
        packet = make_packet(packets[i]);
        arrival_time = packet.arrival_time;
        if (transmission_time >= packet.arrival_time)
        {
            add_packet(packet);
            i += 1;
            continue;
        }
        // if the output line is free, transmit all packets that can be transmitted.
        send_packets(arrival_time, service_rate);
        // check if all queues are empty.
        bool all_empty = true;
        for (auto id : queue_ids)
            if (queues[id].size() > 0)
            {
                all_empty = false;
                break;
            }
        // keep track of the max queue id of all packets with the same arrival time.
        int max_queue_id = -1;
        // keep track of number of packets with the same arrival time.
        int n_packets = 0;
        // this while loop reads all packets with same arrival times at once before
        // deciding which one to send.
        while (arrival_time == packet.arrival_time)
        {
            // add the current packet to the list of packets.
            add_packet(packet);
            max_queue_id = max(max_queue_id, packet.queue_id);
            n_packets += 1; // increment the number of packets with the same arrival time.
            i += 1;
            if (i >= packets.size())
                break;
            // create a packet from the next line.
            packet = make_packet(packets[i]);
        }
        if (all_empty && n_packets > 1 && transmission_time < arrival_time)
        {
            // if all queues are empty and there are more than one packets with the same arrival time,
            // send the packet with the max queue id. This is the tie-breaker.
            cur_queue_id = prev_queue(max_queue_id);
        }
    }
    while (true) // this loop is to transmit the remaining packets in all the queues.
    {
        Packet to_transmit = peek_packet(); // get a packet in round-robin fashion.
        int queue_id = to_transmit.queue_id;
        if (queue_id == -1)
            break;                           // if no packets are in the queues, stop sending
        remove_packet(queue_id);             // if there is a packet, remove it from the queue.
        transmit(to_transmit, service_rate); // transmit the packet on the output line.
    }
    return 0;
}
