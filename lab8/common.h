/**
 * This file contains some common macros and imports used by all the 
 * other files. 
 * Some functions are defined as macros to output better messages.
 */

#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>

// max size of the buffer
#define MAX 1024
/**
 * These ports defines the port on which each of the required
 * processes run.
 */
#define ADD_PORT 12500
#define SUB_PORT 12501
#define MUL_PORT 12502
#define IDIV_PORT 12503
// The port on which h3 accepts UDP connections from h2.
#define H3_PORT 12504
// IP address of h2.
#define H2_IP "192.168.1.3"
// IP address of h3.
#define H3_IP "192.168.1.4"

using namespace std;

/**
 * Populate the address struct with the given IP and port.
 */
void get_addr(sockaddr_in &addr, const char *ip, int port)
{
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (ip == NULL)
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        addr.sin_addr.s_addr = inet_addr(ip);
    }
}

/**
 * Given a string, it returns a list of strings, 
 * delimited by the given delimiter.
 */
void tokenize(vector<string> &vec, string str, string delim)
{
    int start = 0;
    int end = str.find(delim);
    while (end != -1)
    {
        vec.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delim, start);
    }
    vec.push_back(str.substr(start, end - start));
}

/**
 * Try to bind the socket to the given address and port.
 * This ensures that we don't bind to random ports.
 */
#define try_bind(sockfd, addr)                               \
    if (bind(sockfd, (sockaddr *)&addr, sizeof(addr)) == -1) \
    {                                                        \
        cerr << ID << "Error binding to socket." << endl;    \
        exit(EXIT_FAILURE);                                  \
    }

/**
 * Try to accept a connection on the given socket. If the connection
 * fails, exit the program.
 */
#define try_accept(new_sockfd, sockfd, addr, addr_size)                      \
    new_sockfd = accept(sockfd, (sockaddr *)&addr, (socklen_t *)&addr_size); \
    if (new_sockfd == -1)                                                    \
    {                                                                        \
        cerr << ID << "Error accepting connection." << endl;                 \
        exit(EXIT_FAILURE);                                                  \
    }

/**
 * Try to create a socket. If it fails, exit the program.
 */
#define try_socket_creation(sockfd, tcp)                         \
    sockfd = socket(AF_INET, tcp ? SOCK_STREAM : SOCK_DGRAM, 0); \
    if (sockfd < 0)                                              \
    {                                                            \
        cerr << ID << "Error creating socket." << endl;          \
        exit(EXIT_FAILURE);                                      \
    }                                                            \
    /* Set socket options, see next macro */                     \
    set_socket_options(sockfd);

/** 
 * Set the socket options. This ensures we can reconnect to the socket as 
 * soon we end the connection, rather that waiting till the timeout.
 */
int opt;
#define set_socket_options(sockfd)                                                      \
    opt = 1;                                                                            \
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) \
    {                                                                                   \
        cerr << ID << "Error setting socket options." << endl;                          \
        exit(EXIT_FAILURE);                                                             \
    }
