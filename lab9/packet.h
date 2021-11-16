#include <bits/stdc++.h>

using namespace std;

// this file stores the valid fragments obtained by q2.
#define VALID_FRAGS_LOC "./valid_fragment_files.txt"

// packet structure
typedef struct IPPacket_t
{
    unsigned char v_hl;
    unsigned char dscp_ecn;
    unsigned short int totalLen;
    unsigned short int id;
    unsigned short int flags_frag_offset;
    unsigned char ttl;
    unsigned char proto;
    unsigned short int checksum;
    unsigned char sAddr[4];
    unsigned char dAddr[4];
    unsigned int o1;
    unsigned int o2;
    unsigned char data[1024];
} IPPacket;

// converts unsigned char to bitset<8>
#define uc2b(x) bitset<8>(x).to_string()
// converts unsigned short int to bitset<16>
#define usi2b(x) bitset<16>(x).to_string()
// converts unsigned int to bitset<32>
#define ui2b(x) bitset<32>(x).to_string()
// converts unsigned char array to binary string
inline string uca2b(const unsigned char *x, int n)
{
    stringstream ss;
    for (int i = 0; i < n; i++)
    {
        ss << uc2b(x[i]);
    }
    return ss.str();
}
/**
 * @brief Prints the packet headers.
 */
string get_headers(const IPPacket &packet)
{
    stringstream buffer;
    // version and header length
    int v_hl = stoi(uc2b(packet.v_hl), nullptr, 2);
    int version = v_hl >> 4;
    int header_len = v_hl & 0x0F;
    // type of service
    int dscp_ecn = stoi(uc2b(packet.dscp_ecn), nullptr, 2);
    int dscp = dscp_ecn >> 2;
    int ecn = dscp_ecn & 0x03;
    // total length of packet
    int total_len = stoi(usi2b(packet.totalLen), nullptr, 2);
    // packet identifier
    int id = stoi(usi2b(packet.id), nullptr, 2);
    // flags and fragment offset
    int flags_frag_offset = stoi(usi2b(packet.flags_frag_offset), nullptr, 2);
    int flags = flags_frag_offset >> 13;
    // first 13 bits are the offset
    int frag_offset = flags_frag_offset & 0x1FFF;
    int ttl = stoi(uc2b(packet.ttl), nullptr, 2);
    // Upper layer protocol
    int proto = stoi(uc2b(packet.proto), nullptr, 2);
    // checksum
    int checksum = stoi(usi2b(packet.checksum), nullptr, 2);
    int sAddr[4];
    int dAddr[4];
    // Source address
    sAddr[0] = stoi(ui2b(packet.sAddr[0]), nullptr, 2);
    sAddr[1] = stoi(ui2b(packet.sAddr[1]), nullptr, 2);
    sAddr[2] = stoi(ui2b(packet.sAddr[2]), nullptr, 2);
    sAddr[3] = stoi(ui2b(packet.sAddr[3]), nullptr, 2);
    // Destination address
    dAddr[0] = stoi(ui2b(packet.dAddr[0]), nullptr, 2);
    dAddr[1] = stoi(ui2b(packet.dAddr[1]), nullptr, 2);
    dAddr[2] = stoi(ui2b(packet.dAddr[2]), nullptr, 2);
    dAddr[3] = stoi(ui2b(packet.dAddr[3]), nullptr, 2);
    buffer << "Version: " << version << endl
           << "Header Length: " << header_len * 4 << " Bytes" << endl
           << "Datagram Length: " << total_len << endl
           << "Identifier: " << id << endl
           << "Flags: " << flags << endl
           << "    Reserved bit: " << (flags & 0x1 ? "yes" : "no") << endl // check flag bits
           << "    Don't fragment: " << (flags & 0x2 ? "yes" : "no") << endl
           << "    More frags: " << (flags & 0x4 ? "yes" : "no") << endl
           << "Fragmentation offset: " << frag_offset << endl
           << "Time to Live (TTL): " << ttl << endl
           << "Upper-Layer Protocol: "
           << "EGP (8)" << endl
           << "Header Checksum: " << checksum << endl
           << "Source IP address: " << sAddr[0] << "." << sAddr[1] << "." << sAddr[2] << "." << sAddr[3] << endl
           << "Destination IP address: " << dAddr[0] << "." << dAddr[1] << "." << dAddr[2] << "." << dAddr[3] << endl;
    return buffer.str();
}