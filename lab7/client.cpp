/**
 * This file contains the client code.
 * To copy this file and server.cpp to their respective machines, run "make copy".
 * To compile this file on the h1, run "make client".
*/

#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

using namespace std;

// These are the flag and escape bytes to be used for byte-stuffing
string FLAG = "1";
string ESC = "0";

// find the value of r using the inequality
int find_redundant_bitcount(int m)
{
    int r = 0;
    while (pow(2, r) < m + r + 1)
    {
        r++;
    }
    return r;
}

// given a string, returns the 8-bit binary representation of the string
string get_binary_repr(const string &s)
{
    string res = "";
    for (int i = 0; i < s.size(); i++)
    {
        // convert the ascii value to 8-bit binary
        bitset<8> ascii_bin((int)s[i]);
        // append the binary value to the result
        res += ascii_bin.to_string();
    }
    return res;
}

/**
 * given a binary string, returns the string encoded using 
 * hamming codes
*/
string get_hamming_code(string s, int m, int r)
{
    int n = m + r;
    // initialize the hamming code with 0s
    string hamming_code(n, '0');
    // fills all the non-parity bits with their corresponding bits.
    for (int i = 0, k = 0; i < n; i++)
    {
        float temp = log2(i + 1);
        // Check if temp is a power of 2.
        if (ceil(temp) != floor(temp))
        {
            hamming_code[i] = s[k];
            k++;
        }
    }
    // fill the parity bits.
    for (int i = 0; i < r; i++)
    {
        // current position of the parity bit
        int position = (int)pow(2, i);
        int counter = position - 1;
        int parity_counter = 0;
        while (counter < n)
        {
            for (int j = counter; j < counter + position && j < n; j++)
            {
                // if the bit is 1, increment the parity counter
                if (hamming_code[j] == '1')
                    parity_counter++;
            }
            // moves to the next group of parity bits to be checked.
            counter += 2 * position;
        }
        // if even parity, set the parity bit to 0
        if (parity_counter % 2 == 0)
            hamming_code[position - 1] = '0';
        // if odd parity, set the parity bit to 1
        else
            hamming_code[position - 1] = '1';
    }
    return hamming_code;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: ./client <string> <integer>\n";
        exit(0);
    }
    int h1_socket;
    // address structure of h2.
    struct sockaddr_in h2_address;
    // create a UDP socket (SOCK_DGRAM)
    if ((h1_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        cerr << "Could not create socket\n";
        exit(EXIT_FAILURE);
    }
    memset(&h2_address, 0, sizeof(h2_address));

    h2_address.sin_family = AF_INET;
    // specify ip of h2
    h2_address.sin_addr.s_addr = htonl(INADDR_ANY);
    // specify port of h2
    h2_address.sin_port = htons(8567);

    // string to be sent
    string s(argv[1]);
    // number of bits in a frame
    int m = atoi(argv[2]);
    // number of redundant bits using the inequality
    int r = find_redundant_bitcount(m);
    // pad the flag byte
    FLAG = string(m - 1, '0') + FLAG;
    // pad the esc byte
    ESC = string(m - 1, '0') + ESC;
    // encode the flag byte
    string FLAG_HAMMING = get_hamming_code(FLAG, m, r);
    // encode the esc byte
    string ESC_HAMMING = get_hamming_code(ESC, m, r);
    // get the binary representation of the string
    string code = get_binary_repr(s);
    cout << code << "\n";
    cout << "Number of parity bits: " << r << "\n";
    cout << "Sent FLAG frame: " << FLAG_HAMMING << "\n";
    // send the flag frame
    sendto(h1_socket, FLAG_HAMMING.c_str(), FLAG_HAMMING.size(), 0, (struct sockaddr *)&h2_address, sizeof(h2_address));
    for (int i = 0; i < code.size(); i += m)
    {
        // get the next m data bits
        string temp = code.substr(i, m);
        // post-padding (this only occurs for the last frame)
        temp = temp + string(m - temp.size(), '0');
        // byte-stuffing if the message contains the flag or esc byte
        if (temp == FLAG || temp == ESC)
        {
            // send the esc frame
            sendto(h1_socket, ESC_HAMMING.c_str(), ESC_HAMMING.size(), 0, (struct sockaddr *)&h2_address, sizeof(h2_address));
            cout << "Sent ESC frame: " << ESC_HAMMING << "\n";
        }
        string message = get_hamming_code(temp, m, r); // encode the data bits
        cout << "Sent frame with content: " << message << "\n";
        sendto(h1_socket, message.c_str(), message.size(), 0, (struct sockaddr *)&h2_address, sizeof(h2_address));
    }
    // finsh by sending the flag frame again.
    cout << "Sent FLAG frame: " << FLAG_HAMMING << "\n";
    sendto(h1_socket, FLAG_HAMMING.c_str(), FLAG_HAMMING.size(), 0, (struct sockaddr *)&h2_address, sizeof(h2_address));
    return 0;
}