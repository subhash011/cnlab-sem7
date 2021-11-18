#include <bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

ulong packet_count = 1;

/**
 * @brief Returns the MAC address, given the ether_addr struct
 * @param addr The ether_addr struct
 */
string ether_ntoa(const struct ether_addr addr)
{
    char buffer[18];
    // convert each octet to hex
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
            addr.ether_addr_octet[0], addr.ether_addr_octet[1],
            addr.ether_addr_octet[2], addr.ether_addr_octet[3],
            addr.ether_addr_octet[4], addr.ether_addr_octet[5]);
    return string(buffer);
}

/**
 * @brief Prints the ethernet headers
 * @param eth The ethernet header struct
 */
void print_ethernet_headers(ethhdr *eth)
{
    ether_addr src_addr, dst_addr;
    // Copy the source and destination MAC addresses
    memcpy(&src_addr, eth->h_source, sizeof(src_addr));
    memcpy(&dst_addr, eth->h_dest, sizeof(dst_addr));
    // Get the protocol from the ethernet header
    protoent *proto = getprotobynumber(eth->h_proto);
    string protocol = proto ? proto->p_name : "unknown";
    // convert the protocol to capital letters
    transform(protocol.begin(), protocol.end(), protocol.begin(), ::toupper);
    cout << "-> Ethernet Headers" << endl;
    cout << "   |- Protocol: " << protocol << endl;
    cout << "   |- Source MAC address: " << ether_ntoa(src_addr) << endl;
    cout << "   |- Destination MAC address: " << ether_ntoa(dst_addr) << endl;
}

/**
 * @brief Prints the IP headers
 * @param iph The IP header struct
 */
void print_ip_headers(iphdr *ip)
{
    in_addr src_ip, dest_ip;
    // Copy the source and destination IP addresses
    src_ip.s_addr = ip->saddr;
    dest_ip.s_addr = ip->daddr;
    // Get the protocol from the IP header
    protoent *proto = getprotobynumber(ip->protocol);
    string protocol = proto ? proto->p_name : "unknown";
    // convert the protocol to capital letters
    transform(protocol.begin(), protocol.end(), protocol.begin(), ::toupper);
    cout << "-> IP Headers" << endl;
    cout << "   |- Protocol: " << protocol << endl;
    cout << "   |- Source IP address: " << inet_ntoa(src_ip) << endl;
    cout << "   |- Destination IP address: " << inet_ntoa(dest_ip) << endl;
}

/**
 * @brief Reads ip headers and ethernet headers from the given buffer
 * and prints them out.
 * @param buffer A character buffer containing the packet
 */
void log(uint8_t *buffer)
{
    // IP headers come after the ethernet headers so offset the
    // pointer by the size of the ethernet headers.
    iphdr *iphdr = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    ethhdr *ethhdr = (struct ethhdr *)buffer;
    cout << "[Packet #" << packet_count++ << "]" << endl;
    print_ethernet_headers(ethhdr);
    print_ip_headers(iphdr);
    cout << endl;
}