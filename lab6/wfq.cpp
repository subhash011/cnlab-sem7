#include <bits/stdc++.h>

using namespace std;

class Packet
{
public:
    int id;
    int size;
    float arrival_time;
    int queue_id;
    float finish_time;

    Packet() : queue_id(-1) {}

    Packet(int id, int size, float arrival_time, int queue_id, float finish_time)
    {
        this->id = id;
        this->size = size;
        this->arrival_time = arrival_time;
        this->queue_id = queue_id;
        this->finish_time = finish_time;
    }
};

// a comparator to compare two packets based on their finish times,
// this is used to store the packets in a sorter order.
bool packet_comparator(Packet p1, Packet p2)
{
    bool id_cmp = p1.id < p2.id;
    bool finish_cmp = p1.finish_time < p2.finish_time;
    bool arrival_cmp = p1.arrival_time < p2.arrival_time;
    if (p1.finish_time == p2.finish_time)
    {
        if (p1.arrival_time == p2.arrival_time)
        {
            return id_cmp;
        }
        return arrival_cmp;
    }
    return finish_cmp;
}

// create a set of packets with the above comparator.
// this is similar to a priority queue and it can guarantee
// the lookup of packet with min finish time in O(1)
set<Packet, decltype(&packet_comparator)> packets_sorted(&packet_comparator);
vector<float> weights;           // the weights of each queues.
vector<float> prev_finish_times; // the previous finish times of each queue.
float transmission_time = 0;     // the transmission time of the current packet in the output line.

// get the finish time as per the formula.
float get_finish_time(int queue_id, float arrival_time, int packet_length)
{
    float finish_time = max(prev_finish_times[queue_id - 1], arrival_time) +
                        (packet_length / weights[queue_id - 1]);
    prev_finish_times[queue_id - 1] = finish_time; // update the previous finish time of this queue.
    return finish_time;
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
    float finish_time = get_finish_time(queue_id, arrival_time, size); // get the finish time.
    return Packet(id, size, arrival_time, queue_id, finish_time);      // create packet and return it.
}

void transmit(Packet packet, float service_rate)
{
    // transmission starts either from the arrival time if the output line is free or
    // the time at which the output line gets freed.
    transmission_time = max(transmission_time, packet.arrival_time) + ((float)packet.size / service_rate);
    printf("%.2f %d %d\n", transmission_time, packet.id, packet.queue_id);
}

// send a packet over the output line.
void send_packets(float arrival_time, float service_rate)
{
    while (true)
    {
        if (packets_sorted.empty())
            break;                                    // if no packets to transmit, stop.
        Packet p = *packets_sorted.begin();           // get the packet with the least finish time.
        packets_sorted.erase(packets_sorted.begin()); // remove the packet from the set.
        transmit(p, service_rate);                    // transmit the packet.
        if (transmission_time >= arrival_time)
            break; // if the transmission time has exceeded the current arrival time, we break
    }
}

int main(int argc, char const *argv[])
{
    if (argc <= 2)
    {
        printf("Usage: ./wfq <servicerate> <weight of queues>.... < <input_file>\n");
        return 0;
    }
    float service_rate = stof(argv[1]);
    for (int i = 2; i < argc; i++)
    {
        // read weights and initialise previous finish times.
        weights.push_back(stof(argv[i]));
        prev_finish_times.push_back(-1);
    }

    float arrival_time;
    string line;
    vector<string> packets;
    while (getline(cin, line))
        packets.push_back(line); // read the file as a list of strings.
    int i = 0;
    while (i < packets.size())
    {
        Packet packet = make_packet(packets[i]); // create a packet from this line.
        arrival_time = packet.arrival_time;
        if (transmission_time >= packet.arrival_time)
        {
            // if output line is busy transmitting, add the packet to the list,
            // sorted by its finish times in asc. order and continue.
            packets_sorted.insert(packet);
            i += 1;
            continue;
        }
        // if the output line is free, transmit all packets that can be transmitted.
        send_packets(arrival_time, service_rate);
        // add this packet to the list of packets
        packets_sorted.insert(packet);
        i += 1;
    }
    while (1) // this loop is to transmit the remaining packets.
    {
        if (packets_sorted.empty())
            break;                                    // if no packets to transmit, stop.
        Packet p = *packets_sorted.begin();           // get the packet with the least finish time.
        packets_sorted.erase(packets_sorted.begin()); // remove the packet from the set.
        transmit(p, service_rate);                    // transmit the packet.
    }

    return 0;
}
