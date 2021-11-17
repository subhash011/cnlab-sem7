#include "common.h"

ulong packet_count = 1;
string eth1_ip;

void log(uint8_t *buffer)
{
    iphdr *iphdr = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    ethhdr *ethhdr = (struct ethhdr *)buffer;
    in_addr src_ip, dst_ip;
    src_ip.s_addr = iphdr->saddr;
    dst_ip.s_addr = iphdr->daddr;
    if (inet_ntoa(src_ip) == eth1_ip)
        cout << "[Packet #" << packet_count++ << " - Outgoing packet]" << endl;
    else if (inet_ntoa(dst_ip) == eth1_ip)
        cout << "[Packet #" << packet_count++ << " - Incoming packet]" << endl;
    else
        cout << "[Packet #" << packet_count++ << " - FORWARDING]" << endl;
    print_ethernet_headers(ethhdr);
    print_ip_headers(iphdr);
    cout << endl;
}

int main()
{
    struct ifreq ifr;
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0)
    {
        cerr << "Socket error" << endl;
        exit(EXIT_FAILURE);
    }
    eth1_ip = ifreq_to_ipstr(get_all_ips()["eth1"]);
    sockaddr_ll sll;
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex("eth1");
    sll.sll_protocol = htons(ETH_P_ALL);
    if (bind(sockfd, (sockaddr *)&sll, sizeof(sockaddr_ll)) < 0)
    {
        cerr << "Bind error: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
    uint8_t *buffer = new uint8_t[65536];
    while (1)
    {
        memset(buffer, 0, 65536);
        int len = recvfrom(sockfd, buffer, 65536, 0, nullptr, nullptr);
        if (len < 0)
        {
            cerr << "Recvfrom error" << endl;
            exit(EXIT_FAILURE);
        }
        log(buffer);
        cout << endl;
    }
    return 0;
}