#include "common.h"

unordered_set<string> local_ips;
ulong packet_count = 1;

void log(uint8_t *buffer)
{
    iphdr *iphdr = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    ethhdr *ethhdr = (struct ethhdr *)buffer;
    in_addr src_ip, dst_ip;
    src_ip.s_addr = iphdr->saddr;
    dst_ip.s_addr = iphdr->daddr;
    if (local_ips.find(inet_ntoa(src_ip)) == local_ips.end())
        cout << "[Packet #" << packet_count << " - Incoming packet]" << endl;
    else if (local_ips.find(inet_ntoa(dst_ip)) == local_ips.end())
        cout << "[Packet #" << packet_count << " - Outgoing packet]" << endl;
    else
        cout << "[Packet #" << packet_count << " - FORWARDING]" << endl;
    print_ethernet_headers(ethhdr);
    print_ip_headers(iphdr);
    cout << endl;
}

int main()
{
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0)
    {
        cerr << "Socket error" << endl;
        exit(EXIT_FAILURE);
    }
    unordered_map<string, addr_info> ips = get_all_ips();
    transform(ips.begin(), ips.end(), inserter(local_ips, local_ips.begin()), [](auto pair)
              { return ifreq_to_ipstr(pair.second); });
    ulong packet_count = 1;
    uint8_t *buffer = new uint8_t[65536];
    struct sockaddr saddr;
    socklen_t saddr_len = sizeof(saddr);
    while (1)
    {
        memset(buffer, 0, 65536);
        int len = recvfrom(sockfd, buffer, 65536, 0, &saddr, &saddr_len);
        if (len < 0)
        {
            cerr << "Recvfrom error" << endl;
            exit(EXIT_FAILURE);
        }
        log(buffer);
        packet_count++;
    }
    return 0;
}