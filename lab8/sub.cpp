#include "common.h"

#define ID "[SUB] "
#define PORT SUB_PORT

string sub(string num1, string num2)
{
    int n1 = stoi(num1);
    int n2 = stoi(num2);
    return to_string(n1 - n2);
}

int main(int argc, char *argv[])
{
    /* buffer to send and receive messages with */
    char buffer[MAX];
    int client_sd, server_sd, h3_sd;
    sockaddr_in client_addr, server_addr, h3_addr;
    socklen_t addr_size = sizeof(client_addr);
    string num1, num2;
    /* Connect to a socket on h2 depending on the operation */
    try_socket_creation(server_sd, true);
    /* Socket for UDP communication with h3. */
    try_socket_creation(h3_sd, false);
    /* Socket addresses for h2 (depending on the operation) and h3 */
    get_addr(server_addr, H2_IP, PORT);
    get_addr(h3_addr, H3_IP, H3_PORT);
    /* Bind the socket to the address */
    try_bind(server_sd, server_addr);
    /* listen for connections from client */
    listen(server_sd, 5);
    while (1)
    {
        cout << ID << "Waiting for client connections..." << endl;
        /* accept a connection */
        try_accept(client_sd, server_sd, client_addr, addr_size);
        string client_ip = inet_ntoa(client_addr.sin_addr);
        cout << "> Connected to client with IP: " << client_ip << endl;
        /* Receive message from client */
        memset(&buffer, 0, sizeof(buffer));
        recv(client_sd, (char *)&buffer, sizeof(buffer), 0);
        cout << "> Message from h1: " << buffer << endl;
        /* Get the two numbers delimited by colon and the port on which h1 will listen. */
        vector<string> tokens;
        string str(buffer), delim(":");
        tokenize(tokens, str, delim);
        string client_port = tokens[2];
        cout << "> Received numbers " << tokens[0] << " and " << tokens[1] << endl;
        /**
         * Perform the appropriate operation, send the IP and port of the client
         * along with the result of the operation to h3, delimited by colon.
         */
        string result = sub(tokens[0], tokens[1]);
        cout << "> Operation: " << ID << "| Result: " << result << endl;
        string message = client_ip + ":" + tokens[2] + ":" + result;
        sendto(h3_sd, message.c_str(), message.size(), 0, (sockaddr *)&h3_addr, addr_size);
        cout << "> Message to h3: " << message << endl;
        close(client_sd);
    }
    close(h3_sd);
    close(server_sd);
    return 0;
}