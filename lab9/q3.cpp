#include "packet.h"

/**
 * @brief This class is used to define the custom comparator for the priority queue used
 * to store and retrieve the fragments in the required order.
 */
class CompareFrags
{
public:
    /**
     * @brief This function is used to compare the two fragments as follows:
     *        - If either of the fragments are the last fragments, they have least priority.
     *        - If fragment offsets are the same, the fragment with the smaller length is prioritized.
     *        - By default, the fragments are ordered by their offsets.
     * @param a The first fragment to be compared.
     * @param b The second fragment to be compared.
     * @return True if the first fragment is less than the second fragment.
     */
    bool operator()(const IPPacket &a, const IPPacket &b)
    {
        bitset<16> a_bits(a.flags_frag_offset);
        bitset<16> b_bits(b.flags_frag_offset);
        // fragment is the last fragment so least priority
        if (!a_bits.test(15))
            return true;
        if (!b_bits.test(15))
            return false;
        a_bits.set(15, 0);
        b_bits.set(15, 0);
        if (a_bits.to_ulong() == b_bits.to_ulong())
            // fragment offsets are the same, the fragment with the smaller length is prioritized
            return a.totalLen > b.totalLen;
        // fragment with the smaller offset is prioritized
        return a_bits.to_ulong() > b_bits.to_ulong();
    }
};

int main(int argc, char const *argv[])
{
    ifstream inFile(VALID_FRAGS_LOC);
    vector<string> files;
    string line;
    while (getline(inFile, line))
    {
        // read all the valid fragment file names from the file
        files.push_back(line);
    }
    inFile.close();
    // map of packet id and priority queue of fragments
    map<unsigned short int, priority_queue<IPPacket, vector<IPPacket>, CompareFrags>> packets;
    int num_no_frags = 0;
    // for each fragment file
    for (auto &file : files)
    {
        IPPacket *packet = new IPPacket();
        // open and read the file in binary mode into the packet structure
        ifstream frag_fl(file, ios::binary);
        frag_fl.read((char *)packet, sizeof(IPPacket));
        frag_fl.close();
        // depending on the packet id, store the fragment in the corresponding priority queue
        packets[packet->id].push(*packet);
    }
    // number of packets is equal to the number of key, value pairs in the map
    cout << "\nTotal number of IPv4 packets: " << packets.size() << "\n"
         << endl;
    // for each packet
    for (auto &packet : packets)
    {
        char *res = new char[65536];
        unsigned short int id = packet.first;
        auto fragments = packet.second;
        ulong total_len = 0;
        ulong total_data_len = 0;
        int num_frags = 0;
        // until all the fragments are retrieved
        while (!fragments.empty())
        {
            IPPacket fragment = fragments.top();
            // this offset represents the offset of the data in the original packet
            // and it is to be multiplied by 8 to get the actual offset in bytes.
            int offset = fragment.flags_frag_offset & 0xFFF;
            // find the header length of the fragment from v_hl
            int header_len = (fragment.v_hl & 0x0F) * 4;
            // this length represents the length of the data in the fragment that
            // is to be copied into the original data buffer.
            int data_len = fragment.totalLen - header_len;
            total_len += fragment.totalLen;
            total_data_len += data_len;
            // copy the data from the fragment into the original data buffer
            // at the offset calculated above.
            memcpy(res + offset * 8, fragment.data, data_len);
            num_frags++;
            // remove the fragment from the priority queue
            fragments.pop();
        }
        cout << "Packet in " << num_frags << " fragments" << endl;
        cout << "ID: " << id << endl;
        cout << "Packet size: " << total_data_len + 28 << " Bytes" << endl;
        cout << "Data size: " << total_data_len << " Bytes" << endl;
        cout << "Data: " << res << "\n"
             << endl;
    }
    return 0;
}
