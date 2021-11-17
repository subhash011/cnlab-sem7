#include "common.h"

ulong packet_count = 1;
string eth1_ip, eth2_ip;

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
    struct ifreq ifr;
    int eth1_sd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    int eth2_sd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (eth1_sd < 0 || eth2_sd < 0)
    {
        cerr << "Socket error" << endl;
        exit(EXIT_FAILURE);
    }
    eth1_ip = ifreq_to_ipstr(get_all_ips()["eth1"]);
    sockaddr_ll eth1_sll, eth2_sll;
    socklen_t sll_len = sizeof(eth1_sll);
    eth1_sll.sll_family = eth2_sll.sll_family = AF_PACKET;
    eth1_sll.sll_ifindex = if_nametoindex("eth1");
    eth2_sll.sll_ifindex = if_nametoindex("eth2");
    eth1_sll.sll_protocol = eth2_sll.sll_protocol = htons(ETH_P_ALL);
    if (bind(eth1_sd, (sockaddr *)&eth1_sll, sll_len) < 0)
    {
        cerr << "Bind error (eth1): " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
    if (bind(eth2_sd, (sockaddr *)&eth2_sll, sll_len) < 0)
    {
        cerr << "Bind error (eth2): " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
    uint8_t *buffer = new uint8_t[65536];
    struct sockaddr_ll saddr;
    socklen_t saddr_len = sizeof(saddr);
    while (1)
    {
        memset(buffer, 0, 65536);
        int len = recvfrom(eth1_sd, buffer, 65536, 0, (sockaddr *)&saddr, &saddr_len);
        if (len < 0)
        {
            cerr << "Recvfrom error" << endl;
            exit(EXIT_FAILURE);
        }
        forward(buffer);
        iphdr *iphdr = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        ethhdr *ethhdr = (struct ethhdr *)buffer;
        uint32_t ip = iphdr->saddr;
        struct sockaddr_ll sadr_ll;
        sadr_ll.sll_ifindex = eth2_sll.sll_ifindex;
        sadr_ll.sll_halen = ETH_ALEN;
        sadr_ll.sll_family = AF_PACKET;
        memcpy(sadr_ll.sll_addr, ethhdr->h_source, ETH_ALEN);
        int sd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
        if (sendto(sd, buffer, len, 0, (sockaddr *)&sadr_ll, sizeof(sadr_ll)) < 0)
        {
            cerr << "sendto error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        cout << endl;
        packet_count++;
    }
    return 0;
}