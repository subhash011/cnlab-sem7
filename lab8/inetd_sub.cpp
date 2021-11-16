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
    char buffer[MAX];
    int h3_sd;
    sockaddr_in h3_addr;
    socklen_t addr_size = sizeof(h3_addr);
    memset(&buffer, 0, sizeof(buffer));
    /** 
     * Since we forwarded all socket messages to the stdin before
     * exec, we can read any input from stdin.
     */
    for (int i = 0; i < MAX; i++)
    {
        buffer[i] = getchar();
        if (buffer[i] == 'E')
        {
            buffer[i + 1] = '\0';
            break;
        }
    }
    vector<string> numbers;
    tokenize(numbers, buffer, ":");
    /* Get the two numbers from stdin. */
    string num1 = numbers[0];
    string num2 = numbers[1];
    string client_ip = argv[1];
    string client_port = numbers[2];
    cout << "> Message from client: " << buffer << endl;
    cout << "> Received numbers " << num1 << " and " << num2 << endl;
    /**
     * Perform the appropriate operation, send the IP and port of the client
     * along with the result of the operation to h3, delimited by colon.
     */
    string oper = sub(num1, num2);
    cout << "> Operation: " << ID << "| Result: " << oper << endl;
    /* Send the IP, port and the result of the operation to h3. */
    string result = client_ip + ":" + client_port + ":" + oper;
    /* Create a UDP socket to send data to h3. */
    try_socket_creation(h3_sd, false);
    get_addr(h3_addr, H3_IP, H3_PORT);
    sendto(h3_sd, result.c_str(), result.size(), 0, (sockaddr *)&h3_addr, addr_size);
    cout << "> Message to h3: " << result << endl;
    return 0;
}