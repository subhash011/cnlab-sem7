#include "common.h"

int main()
{
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0)
    {
        cerr << "Socket error" << endl;
        exit(EXIT_FAILURE);
    }
    string local_ip = ifreq_to_ipstr(get_all_ips()["eth1"]);
    cout << local_ip << endl;
    ulong packet_count = 1;
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
        iphdr *iphdr = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        ethhdr *ethhdr = (struct ethhdr *)buffer;
        in_addr src_ip, dst_ip;
        src_ip.s_addr = iphdr->saddr;
        dst_ip.s_addr = iphdr->daddr;

        if (inet_ntoa(src_ip) != local_ip && inet_ntoa(dst_ip) != local_ip)
            continue;
        if (inet_ntoa(src_ip) == local_ip)
            cout << "[Packet #" << packet_count << ": Outgoing packet]" << endl;
        else
            cout << "[Packet #" << packet_count << ": Incoming packet]" << endl;
        print_ip_headers(iphdr);
        print_ethernet_headers(ethhdr);
        cout << endl;
        packet_count++;
    }
}