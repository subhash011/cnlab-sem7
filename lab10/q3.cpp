#include "common.h"

#define ETH1 0
#define ETH2 1

ulong packet_count = 1;

void forward(uint8_t *buffer)
{
    iphdr *iphdr = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    ethhdr *ethhdr = (struct ethhdr *)buffer;
    cout << "[Packet #" << packet_count << " - FORWARDING]" << endl;
    print_ethernet_headers(ethhdr);
    print_ip_headers(iphdr);
    cout << endl;
}

int main()
{
    int recv_sds[2], send_sds[2];
    sockaddr_ll recv_sll[2], send_sll[2];
    struct ifreq ifr;
    recv_sds[ETH1] = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    recv_sds[ETH2] = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    send_sds[ETH1] = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
    send_sds[ETH2] = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
    if (recv_sds[ETH1] < 0 || recv_sds[ETH2] < 0)
    {
        cerr << "Socket error" << endl;
        exit(EXIT_FAILURE);
    }
    recv_sll[ETH1].sll_family = recv_sll[ETH2].sll_family = send_sll[ETH1].sll_family = send_sll[ETH2].sll_family = AF_PACKET;
    recv_sll[ETH1].sll_ifindex = send_sll[ETH1].sll_ifindex = if_nametoindex("eth1");
    recv_sll[ETH2].sll_ifindex = send_sll[ETH2].sll_ifindex = if_nametoindex("eth2");
    send_sll[ETH1].sll_halen = send_sll[ETH2].sll_halen = ETH_ALEN;
    recv_sll[ETH1].sll_protocol = recv_sll[ETH2].sll_protocol = htons(ETH_P_ALL);
    if (bind(recv_sds[ETH1], (sockaddr *)&recv_sll[ETH1], sizeof(sockaddr_ll)) < 0)
    {
        cerr << "Bind error (eth1): " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
    if (bind(recv_sds[ETH2], (sockaddr *)&recv_sll[ETH2], sizeof(sockaddr_ll)) < 0)
    {
        cerr << "Bind error (eth2): " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
    uint8_t *buffer = new uint8_t[65536];
    int len;
    ethhdr *ethhdr;
    while (1)
    {
        /*****************************************From h1 to r2**********************************************/
        memset(buffer, 0, 65536);
        len = recvfrom(recv_sds[ETH1], buffer, 65536, 0, nullptr, nullptr);
        if (len < 0)
        {
            cerr << "Recvfrom error" << endl;
            exit(EXIT_FAILURE);
        }
        forward(buffer);
        ethhdr = (struct ethhdr *)buffer;
        memcpy(send_sll[ETH2].sll_addr, ethhdr->h_source, ETH_ALEN);
        if (sendto(send_sds[ETH2], buffer, len, 0, (sockaddr *)&send_sll[ETH2], sizeof(sockaddr_ll)) < 0)
        {
            cerr << "sendto error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        cout << endl;
        packet_count++;

        /*****************************************From r2 to h1**********************************************/
        memset(buffer, 0, 65536);
        len = recvfrom(recv_sds[ETH2], buffer, 65536, 0, nullptr, nullptr);
        if (len < 0)
        {
            cerr << "Recvfrom error" << endl;
            exit(EXIT_FAILURE);
        }
        forward(buffer);
        ethhdr = (struct ethhdr *)buffer;
        memcpy(send_sll[ETH1].sll_addr, ethhdr->h_source, ETH_ALEN);
        if (sendto(send_sds[ETH1], buffer, len, 0, (sockaddr *)&send_sll[ETH1], sizeof(sockaddr_ll)) < 0)
        {
            cerr << "sendto error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        cout << endl;
        packet_count++;

        /*****************************************From h1 to h2**********************************************/
        // memset(buffer, 0, 65536);
        // len = recvfrom(recv_sds[ETH1], buffer, 65536, 0, nullptr, nullptr);
        // if (len < 0)
        // {
        //     cerr << "Recvfrom error" << endl;
        //     exit(EXIT_FAILURE);
        // }
        // forward(buffer);
        // ethhdr = (struct ethhdr *)buffer;
        // memcpy(send_sll[ETH1].sll_addr, ethhdr->h_source, ETH_ALEN);
        // if (sendto(send_sds[ETH1], buffer, len, 0, (sockaddr *)&send_sll[ETH1], sizeof(sockaddr_ll)) < 0)
        // {
        //     cerr << "sendto error: " << strerror(errno) << endl;
        //     exit(EXIT_FAILURE);
        // }
        // cout << endl;
        // packet_count++;
    }
    return 0;
}