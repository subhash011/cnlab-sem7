#include "common.h"

#define ETH1 0
#define ETH2 1
#define closeall()      \
    close(send_sd);     \
    close(recv_sds[0]); \
    close(recv_sds[1]);

uint32_t get_if_ip(string ifname)
{
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    return ((sockaddr_in *)&ifr.ifr_ifru.ifru_addr)->sin_addr.s_addr;
}
int main()
{
    int recv_sds[2], send_sd, len;
    sockaddr_ll recv_slls[2], send_slls[2];
    uint8_t *buffer = new uint8_t[65536];
    ifreq ifr;
    uint32_t eth1_ip = get_if_ip("eth1");
    uint32_t eth2_ip = get_if_ip("eth2");

    // create the sockets, ETH_P_IP tells the socket to only listen to
    // incoming IP packets
    recv_sds[ETH1] = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    recv_sds[ETH2] = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    // socket for sending the packets
    send_sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (recv_sds[ETH1] < 0 || recv_sds[ETH2] < 0 || send_sd < 0)
    {
        cerr << "Socket error: " << strerror(errno) << endl;
        closeall();
        exit(EXIT_FAILURE);
    }
    recv_slls[ETH1] = send_slls[ETH1] = (sockaddr_ll){
        .sll_family = AF_PACKET,
        .sll_protocol = htons(ETH_P_IP),
        .sll_ifindex = (int)if_nametoindex("eth1"), // listens only on eth1
        .sll_halen = ETH_ALEN};
    recv_slls[ETH2] = send_slls[ETH2] = (sockaddr_ll){
        .sll_family = AF_PACKET,
        .sll_protocol = htons(ETH_P_IP),
        .sll_ifindex = (int)if_nametoindex("eth2"), // listens only on eth2
        .sll_halen = ETH_ALEN};
    // bind the sockets to their respective interfaces using the sockaddr_ll struct
    if (bind(recv_sds[ETH1], (sockaddr *)&recv_slls[ETH1], sizeof(sockaddr_ll)) < 0)
    {
        cerr << "Bind error (eth1): " << strerror(errno) << endl;
        closeall();
        exit(EXIT_FAILURE);
    }
    if (bind(recv_sds[ETH2], (sockaddr *)&recv_slls[ETH2], sizeof(sockaddr_ll)) < 0)
    {
        cerr << "Bind error (eth2): " << strerror(errno) << endl;
        closeall();
        exit(EXIT_FAILURE);
    }
    fd_set readfds;
    // clear the readfds set
    FD_ZERO(&readfds);
    // get the max file descriptor
    int max_sd = *max_element(recv_sds, recv_sds + 2);
    while (1)
    {
        memset(buffer, 0, 65536);
        for (int i = 0; i < 2; i++)
        {
            // add the socket to the readfds set
            FD_SET(recv_sds[i], &readfds);
        }
        // wait for any of the two sockets to be ready to read
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            cerr << "Error while calling select: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        bool send_packet = false;
        // if eth1 received a packet, read it
        if (FD_ISSET(recv_sds[ETH1], &readfds))
        {
            len = recvfrom(recv_sds[ETH1], buffer, 65536, 0, nullptr, nullptr);
            send_packet = true;
        }
        // if eth2 received a packet, read it
        if (FD_ISSET(recv_sds[ETH2], &readfds))
        {
            len = recvfrom(recv_sds[ETH2], buffer, 65536, 0, nullptr, nullptr);
            send_packet = true;
        }
        iphdr *ip_hdr = (iphdr *)(buffer + sizeof(ethhdr));
        // if the packet is for eth1 or eth2, ignore it
        if (ip_hdr->daddr == eth1_ip || ip_hdr->daddr == eth2_ip)
        {
            // print the details of the packet
            log(buffer);
            send_packet = false;
        }
        // if the packet was received by some other interface, ignore it
        if (!send_packet)
            continue;
        if (len < 0)
        {
            cerr << "Recvfrom error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        // print the details of the packet
        log(buffer);
        // store the destination IP address in the struct
        sockaddr_in saddr = (sockaddr_in){.sin_family = AF_INET, .sin_addr = {.s_addr = ip_hdr->daddr}};
        // send the packet to the destination using the send_sd socket and the saddr struct
        if (sendto(send_sd, ip_hdr, len - sizeof(ethhdr), 0, (sockaddr *)&saddr, sizeof(sockaddr_in)) < 0)
        {
            cerr << "sendto error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
    }
    // close all sockets
    closeall();
    return 0;
}