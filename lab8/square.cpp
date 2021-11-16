#include "common.h"

#define PORT H3_PORT
#define ID "[H3] "

using namespace std;

string square(string num)
{
    int n = stoi(num);
    int square = n * n;
    return to_string(square);
}

int main(int argc, char const *argv[])
{
    int h3_socket, client_socket;
    char buffer[MAX];
    struct sockaddr_in client_addr, h2_addr, h3_addr;
    socklen_t addr_size = sizeof(client_addr);
    /* create a UDP socket (SOCK_DGRAM) */
    try_socket_creation(h3_socket, false);
    memset(&client_addr, 0, sizeof(client_addr));
    memset(&h2_addr, 0, sizeof(h2_addr));
    memset(&h3_addr, 0, sizeof(PORT));
    /* h3 accepts UDP connections on this port. */
    get_addr(h3_addr, H3_IP, H3_PORT);

    /* Bind the socket of h3 to the ip and port */
    if (bind(h3_socket, (struct sockaddr *)&h3_addr, addr_size) < 0)
    {
        cout << ID << "Could not bind socket." << endl;
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        memset(&buffer, 0, sizeof(buffer));
        cout << ID << "Waiting for message from h2..." << endl;
        /* Receive a message from h2 */
        recvfrom(h3_socket, (char *)&buffer, sizeof(buffer), 0, (struct sockaddr *)&h2_addr, (socklen_t *)&h2_addr);
        /** 
         * Tokenize the message from h2 to get the required messages (delimited by colon). 
         * First parameter is the IP address of the client to which h3 has to send the result.
         * Second parameter is the port on which the client is accepting TCP connections.
         * Third parameter is the result obtained by performing the operation requested by the client.
         */
        vector<string> tokens;
        string str(buffer), delim(":");
        tokenize(tokens, str, delim);
        string result = square(tokens[2]);
        cout << "> Message from h2: " << buffer << endl;
        cout << "> Result from h2: " << tokens[2] << endl;
        cout << "> Final result (squared): " << result << endl;
        /* Tell h3 where the client is */
        get_addr(client_addr, tokens[0].c_str(), stoi(tokens[1]));
        /* Create a TCP socket to connect to client */
        try_socket_creation(client_socket, true);
        /* Try to connect to the client */
        if (connect(client_socket, (sockaddr *)&client_addr, addr_size) < 0)
        {
            cout << ID << "Error connecting to the client!" << endl;
        }
        /* Send the final result to the client. This will be an integer. */
        send(client_socket, result.c_str(), result.length(), 0);
        cout << "> Sent result to IP: " << tokens[0] << " and PORT: " << tokens[1] << endl;
        close(client_socket);
    }
}