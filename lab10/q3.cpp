#include "common.h"

#define ETH1 0
#define ETH2 1
#define ETH1_IP "192.168.20.1"
#define ETH2_IP "192.168.30.1"
#define H1_IP "192.168.20.2"
#define H2_IP "192.168.20.3"
#define R2_IP "192.168.30.2"
uint8_t R2_MAC[ETH_ALEN] = {0x08, 0x00, 0x27, 0xA6, 0xEF, 0x5D};
uint8_t H1_MAC[ETH_ALEN] = {0x08, 0x00, 0x27, 0x63, 0xA5, 0xD5};
uint8_t H2_MAC[ETH_ALEN] = {0x08, 0x00, 0x27, 0xFB, 0x88, 0xE4};

ulong packet_count = 1;

void forward(uint8_t *buffer)
{
    iphdr *ip_hdr = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    ethhdr *ethhdr = (struct ethhdr *)buffer;
    cout << "[Packet #" << packet_count << " - FORWARDING]" << endl;
    print_ethernet_headers(ethhdr);
    print_ip_headers(ip_hdr);
    cout << endl;
}

void get_if_hwaddr(string ifname, uint8_t *hwaddr)
{
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (fd < 0)
    {
        perror("socket");
        exit(1);
    }
    strcpy(ifr.ifr_name, ifname.c_str());
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
    {
        perror("ioctl");
        exit(1);
    }
    close(fd);
    uint8_t *ptr = reinterpret_cast<uint8_t *>(ifr.ifr_hwaddr.sa_data);
    for (int i = 0; i < 6; i++)
        hwaddr[i] = ptr[i];
}

int main()
{
    int recv_sds[2], send_sds[2];
    sockaddr_ll recv_sll[2], send_sll[2];
    uint8_t if_macs[2][ETH_ALEN];
    recv_sds[ETH1] = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    recv_sds[ETH2] = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (recv_sds[ETH1] < 0 || recv_sds[ETH2] < 0)
    {
        cerr << "Socket error" << endl;
        exit(EXIT_FAILURE);
    }
    get_if_hwaddr("eth1", if_macs[ETH1]);
    get_if_hwaddr("eth2", if_macs[ETH2]);
    recv_sll[ETH1].sll_family = send_sll[ETH1].sll_family = AF_PACKET;
    recv_sll[ETH2].sll_family = send_sll[ETH2].sll_family = AF_PACKET;
    recv_sll[ETH1].sll_ifindex = send_sll[ETH1].sll_ifindex = if_nametoindex("eth1");
    recv_sll[ETH2].sll_ifindex = send_sll[ETH2].sll_ifindex = if_nametoindex("eth2");
    recv_sll[ETH1].sll_protocol = recv_sll[ETH2].sll_protocol = htons(ETH_P_IP);
    send_sll[ETH1].sll_halen = send_sll[ETH2].sll_halen = ETH_ALEN;
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
    fd_set readfds;
    FD_ZERO(&readfds);
    uint8_t *buffer = new uint8_t[65536];
    int len;
    ethhdr *eth_hdr;
    iphdr *ip_hdr;
    int max_sd = *max_element(recv_sds, recv_sds + 2);
    while (1)
    {
        for (int i = 0; i < 2; i++)
        {
            FD_SET(recv_sds[i], &readfds);
        }
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            cerr << "Error while calling select: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(recv_sds[ETH1], &readfds))
        {
            memset(buffer, 0, 65536);
            len = recvfrom(recv_sds[ETH1], buffer, 65536, 0, nullptr, nullptr);
            if (len < 0)
            {
                cerr << "Recvfrom error" << endl;
                exit(EXIT_FAILURE);
            }
            ip_hdr = (struct iphdr *)(buffer + sizeof(ethhdr));
            eth_hdr = (struct ethhdr *)buffer;
            if (ip_hdr->daddr == inet_addr(R2_IP))
            {
                memcpy(eth_hdr->h_source, if_macs[ETH2], ETH_ALEN);
                memcpy(eth_hdr->h_dest, R2_MAC, ETH_ALEN);
                memcpy(send_sll[ETH2].sll_addr, R2_MAC, ETH_ALEN);
            }
            else
            {
                continue;
            }
            forward(buffer);
            send_sds[ETH2] = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
            if (sendto(send_sds[ETH2], buffer, len, 0, (sockaddr *)&send_sll[ETH2], sizeof(sockaddr_ll)) < 0)
            {
                cerr << "sendto error: " << strerror(errno) << endl;
                exit(EXIT_FAILURE);
            }
            cout << endl;
            close(send_sds[ETH2]);
            packet_count++;
        }
        if (FD_ISSET(recv_sds[ETH2], &readfds))
        {
            memset(buffer, 0, 65536);
            len = recvfrom(recv_sds[ETH2], buffer, 65536, 0, nullptr, nullptr);
            if (len < 0)
            {
                cerr << "Recvfrom error" << endl;
                exit(EXIT_FAILURE);
            }
            ip_hdr = (struct iphdr *)(buffer + sizeof(ethhdr));
            eth_hdr = (struct ethhdr *)buffer;
            memcpy(eth_hdr->h_source, if_macs[ETH1], ETH_ALEN);
            if (ip_hdr->daddr == inet_addr(H1_IP))
            {
                memcpy(eth_hdr->h_dest, H1_MAC, ETH_ALEN);
                memcpy(send_sll[ETH1].sll_addr, H1_MAC, ETH_ALEN);
            }
            else if (ip_hdr->daddr == inet_addr(H2_IP))
            {
                memcpy(eth_hdr->h_dest, H2_MAC, ETH_ALEN);
                memcpy(send_sll[ETH1].sll_addr, H2_MAC, ETH_ALEN);
            }
            else
            {
                continue;
            }
            forward(buffer);
            send_sds[ETH1] = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
            if (sendto(send_sds[ETH1], buffer, len, 0, (sockaddr *)&send_sll[ETH1], sizeof(sockaddr_ll)) < 0)
            {
                cerr << "sendto error: " << strerror(errno) << endl;
                exit(EXIT_FAILURE);
            }
            close(send_sds[ETH1]);
            cout << endl;
            packet_count++;
        }
    }
    return 0;
}