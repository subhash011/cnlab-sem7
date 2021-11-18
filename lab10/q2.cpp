#include "common.h"

int main()
{
    struct ifreq ifr;
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0)
    {
        cerr << "Socket error" << endl;
        exit(EXIT_FAILURE);
    }
    // This struct is tells the socket to listen to all packets
    // only on the interface eth1
    sockaddr_ll sll;
    sll.sll_family = AF_PACKET;
    // listens only eth1
    sll.sll_ifindex = if_nametoindex("eth1");
    sll.sll_protocol = htons(ETH_P_ALL);
    // bind the socket to the interface using the sockaddr_ll struct
    if (bind(sockfd, (sockaddr *)&sll, sizeof(sockaddr_ll)) < 0)
    {
        cerr << "Bind error: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
    // buffer to store the packet
    uint8_t *buffer = new uint8_t[65536];
    while (1)
    {
        memset(buffer, 0, 65536);
        // read the packet
        int len = recvfrom(sockfd, buffer, 65536, 0, nullptr, nullptr);
        if (len < 0)
        {
            cerr << "Recvfrom error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        // print the details of the packet
        log(buffer);
    }
    close(sockfd);
    return 0;
}