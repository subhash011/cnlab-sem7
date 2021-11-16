#include <bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

using namespace std;

string ether_ntoa(const struct ether_addr addr)
{
    static char buf[18];
    char *buff = new char[18];
    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
            addr.ether_addr_octet[0], addr.ether_addr_octet[1],
            addr.ether_addr_octet[2], addr.ether_addr_octet[3],
            addr.ether_addr_octet[4], addr.ether_addr_octet[5]);
    return string(buf);
}

void print_ethernet_headers(ethhdr *eth)
{
    ether_addr src_addr, dst_addr;
    memcpy(&src_addr, eth->h_source, sizeof(src_addr));
    memcpy(&dst_addr, eth->h_dest, sizeof(dst_addr));
    cout << "Ethernet Header" << endl;
    cout << "Source MAC Address: " << ether_ntoa(src_addr) << endl;
    cout << "Destination MAC Address: " << ether_ntoa(dst_addr) << endl;
}

void print_ip_headers(iphdr *ip)
{
    in_addr src_ip, dest_ip;
    src_ip.s_addr = ip->saddr;
    dest_ip.s_addr = ip->daddr;

    cout << "Protocol: " << (int)ip->protocol << endl;
    cout << "Source IP: " << inet_ntoa(src_ip) << endl;
    cout << "Destination IP: " << inet_ntoa(dest_ip) << endl;
}

int main()
{
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (sockfd < 0)
    {
        cerr << "Socket error" << endl;
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        unsigned char *buffer = new unsigned char[65536];
        struct sockaddr saddr;
        socklen_t saddr_len = sizeof(saddr);
        int len = recvfrom(sockfd, buffer, 65536, 0, &saddr, &saddr_len);
        if (len < 0)
        {
            cerr << "Recvfrom error" << endl;
            exit(EXIT_FAILURE);
        }
        iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        ethhdr *eth = (struct ethhdr *)buffer;
        if (ip->protocol == 1)
        {
            cout << "ICMP Packet" << endl;
            print_ip_headers(ip);
            print_ethernet_headers(eth);
        }
    }
}