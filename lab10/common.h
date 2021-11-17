#include <bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>

using namespace std;

typedef struct addr_info
{
    string if_name;
    sockaddr ip_addr;
    sockaddr mac_addr;
} addr_info;

unordered_map<string, addr_info> get_all_ips()
{
    unordered_map<string, addr_info> ips;
    ifconf ifc;
    ifreq ifr[NI_MAXHOST];
    int addr;
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        cerr << "Socket creation failed" << endl;
        exit(EXIT_FAILURE);
    }
    ifc.ifc_len = sizeof(ifr);
    ifc.ifc_ifcu.ifcu_buf = (caddr_t)ifr;
    if (ioctl(sd, SIOCGIFCONF, &ifc) < 0)
    {
        cerr << "ioctl error: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    int ifc_num = ifc.ifc_len / sizeof(struct ifreq);
    for (int i = 0; i < ifc_num; i++)
    {
        addr_info info;
        addr = ifr[i].ifr_addr.sa_family;
        if (addr != AF_INET)
            continue;
        if (ioctl(sd, SIOCGIFADDR, &ifr[i]) < 0)
        {
            cerr << "ioctl error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        info.ip_addr = ifr[i].ifr_ifru.ifru_addr;
        if (ioctl(sd, SIOCGIFHWADDR, &ifr[i]) < 0)
        {
            cerr << "ioctl error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        info.mac_addr = ifr[i].ifr_ifru.ifru_hwaddr;
        info.if_name = ifr[i].ifr_name;
        ips[ifr[i].ifr_name] = info;
    }
    return ips;
}

string ifreq_to_ipstr(addr_info info)
{
    string ipstr = "";
    sockaddr *addr = &info.ip_addr;
    sockaddr_in *addr_in = (sockaddr_in *)addr;
    ipstr = inet_ntoa(addr_in->sin_addr);
    return ipstr;
}

string ifreq_to_macstr(addr_info info)
{
    char buffer[6];
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
            (unsigned char)info.mac_addr.sa_data[0], (unsigned char)info.mac_addr.sa_data[1],
            (unsigned char)info.mac_addr.sa_data[2], (unsigned char)info.mac_addr.sa_data[3],
            (unsigned char)info.mac_addr.sa_data[4], (unsigned char)info.mac_addr.sa_data[5]);
    return string(buffer);
}

string ether_ntoa(const struct ether_addr addr)
{
    char buffer[18];
    sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
            addr.ether_addr_octet[0], addr.ether_addr_octet[1],
            addr.ether_addr_octet[2], addr.ether_addr_octet[3],
            addr.ether_addr_octet[4], addr.ether_addr_octet[5]);
    return string(buffer);
}

void print_ethernet_headers(ethhdr *eth)
{
    ether_addr src_addr, dst_addr;
    memcpy(&src_addr, eth->h_source, sizeof(src_addr));
    memcpy(&dst_addr, eth->h_dest, sizeof(dst_addr));
    string protocol(getprotobynumber(eth->h_proto)->p_name);
    transform(protocol.begin(), protocol.end(), protocol.begin(), ::toupper);
    cout << "> Ethernet Header Protocol: " << protocol << endl;
    cout << "> MAC Address: " << ether_ntoa(src_addr) << " (src)"
         << " -> " << ether_ntoa(dst_addr) << " (dest)" << endl;
}

void print_ip_headers(iphdr *ip)
{
    in_addr src_ip, dest_ip;
    src_ip.s_addr = ip->saddr;
    dest_ip.s_addr = ip->daddr;
    string protocol(getprotobynumber(ip->protocol)->p_name);
    transform(protocol.begin(), protocol.end(), protocol.begin(), ::toupper);
    cout << "> IP Header Protocol: " << protocol << endl;
    cout << "> IP address: " << inet_ntoa(src_ip) << " (src)"
         << " -> " << inet_ntoa(dest_ip) << " (dest)" << endl;
}