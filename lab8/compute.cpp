/* IP address and port of client is not hard coded unlike other machines. */

#include "common.h"

#define ID "[CLIENT] "

/** 
 * Get a free port for binding the server. 
 * This allows us to be sure that we use a free port.
 */
int get_free_port()
{
    int sockfd;
    sockaddr_in addr;
    socklen_t addr_len(sizeof(addr));
    try_socket_creation(sockfd, true);
    /* bind to any free port. */
    try_bind(sockfd, addr);
    /* populates the address structure */
    getsockname(sockfd, (sockaddr *)&addr, &addr_len);
    close(sockfd);
    return ntohs(addr.sin_port);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cerr << ID << "Usage: ./compute <integer 1> <integer 2> <operation>" << endl;
        cerr << ID << "Operations: ADD, SUB, MUL, IDIV" << endl;
        exit(EXIT_FAILURE);
    }
    /* Define variables */
    char buffer[MAX];
    int client_sd, server_sd, h3_sd;
    int bind_port = get_free_port();
    sockaddr_in h2_addr, server_addr, h3_addr;
    socklen_t addr_size(sizeof(server_addr));
    /* Initialise address structures. */
    memset(&buffer, 0, sizeof(buffer));
    memset(&server_addr, 0, addr_size);
    memset(&h2_addr, 0, addr_size);
    memset(&h3_addr, 0, addr_size);
    /* Depending on the operation argument, decide which port to connect. */
    string op = argv[3];
    transform(op.begin(), op.end(), op.begin(), ::toupper);
    int port;
    if (op == "ADD")
        port = ADD_PORT;
    else if (op == "SUB")
        port = SUB_PORT;
    else if (op == "MUL")
        port = MUL_PORT;
    else if (op == "IDIV")
        port = IDIV_PORT;
    else
    {
        cerr << ID << "Invalid operation, Valid operations are ADD, SUB, MUL and IDIV" << endl;
        exit(EXIT_FAILURE);
    }
    /* Create a socket for the client and the server. */
    try_socket_creation(client_sd, true);
    try_socket_creation(server_sd, true);
    /* Address of h2 to which we need to connect.. */
    get_addr(h2_addr, H2_IP, port);
    /** 
     * NULL implies any interface, so this will accept TCP connection
     * from h3 on any interface. 
     */
    get_addr(server_addr, NULL, bind_port);
    /**
     * Bind server socket to server address.
     * This ensures lets h3 to connect to bind_port.
     */
    try_bind(server_sd, server_addr);
    listen(server_sd, 5);
    cout << ID << "Connected to PORT: " << ntohs(server_addr.sin_port) << endl;
    /* Connect to h2. */
    if (connect(client_sd, (sockaddr *)&h2_addr, addr_size) < 0)
    {
        cout << ID << "Error connecting to server!" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "> Connected to the [" << op << "] server!" << endl;
    /** 
     * Send the numbers to the server along with the 
     * end of message string (:E). This helps to read from 
     * stdin and be sure that the server is not stuck waiting for EOF.
     */
    string s1 = argv[1];
    string s2 = argv[2];
    string res = s1 + ":" + s2 + ":" + to_string(bind_port) + ":" + "E";
    cout << "> Message to h2: " << res << endl;
    send(client_sd, res.c_str(), res.length(), 0);
    /* Accept the connection from h3 */
    try_accept(h3_sd, server_sd, h3_addr, addr_size);
    /* Receive the result from h3 */
    recv(h3_sd, buffer, MAX, 0);
    cout << "> Final result from h3: " << buffer << endl;
    close(client_sd);
    close(server_sd);
    close(h3_sd);
    return 0;
}
