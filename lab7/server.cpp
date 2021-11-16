/**
 * This file contains the server code.
 * To copy this file and client.cpp to their respective machines, run "make copy".
 * To compile this file on the h3, run "make server".
*/

#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

using namespace std;

// These are the flag and escape bytes to be used for byte-stuffing
string FLAG = "1";
string ESC = "0";
bool FLAG_SET = false;

// decodes the hamming code, corrects the errors and returns the original data bits.
string correct_hamming_code(string hamming_code)
{
    int n = hamming_code.size();
    int r = (int)ceil(log2(n + 1));
    // initialise the syndrome to zeros
    string syndrome(r, '0');
    // for each parity position, find the parity
    for (int i = 0; i < r; i++)
    {
        int position = (int)pow(2, i);
        int counter = position - 1;
        int parity_counter = 0;
        // for each bit from the current parity position, check its parity
        while (counter < n)
        {
            for (int j = counter; j < counter + position && j < n; j++)
            {
                // if parity is 1, increment parity counter
                if (hamming_code[j] == '1')
                    parity_counter++;
            }
            counter += 2 * position;
        }
        // change the syndrome bit depending on the parity.
        if (parity_counter % 2 == 0)
            syndrome[i] = '0';
        else
            syndrome[i] = '1';
    }
    // to get the actual syndrome, reverse the string
    reverse(syndrome.begin(), syndrome.end());
    int parity_position = stoi(syndrome, nullptr, 2);
    if (parity_position != 0)
    {
        // flip the error bit
        hamming_code[parity_position - 1] = hamming_code[parity_position - 1] == '0' ? '1' : '0';
    }
    return hamming_code;
}

/** given a hamming code, this function corrects it 
 * and returns the original databits received by h2 
 * from h1.
*/
string retrieve_message(string hamming_code)
{
    // correct the code
    string original_code = correct_hamming_code(hamming_code);
    string data_bits, message = "";
    int m = 0;
    for (int i = 0; i < original_code.size(); i++)
    {
        float temp = log2(i + 1);
        if (ceil(temp) == floor(temp))
        {
            continue;
        }
        m++;
        // get all the databits
        data_bits += original_code[i];
    }
    // pad the flags with the appropriate size if
    // it hasn't been padded for this messsage.
    if (!FLAG_SET)
    {
        FLAG = string(m - 1, '0') + FLAG;
        ESC = string(m - 1, '0') + ESC;
        FLAG_SET = true;
    }
    return data_bits;
}

/** given a binary string, this function
 * reads 8-bits at a time, converts the 8-bit
 * integer to ascii character and returns the complete
 * string. 
 */
string binary_to_string(string binary)
{
    string message = "";
    int len = binary.size();
    for (int i = 0; i < len; i += 8)
    {
        if (len - i < 8)
        {
            // all bits from now are for padding and hence they can be ignored
            break;
        }
        string temp = binary.substr(i, 8);
        // convert binary substring to integer.
        int ascii_value = stoi(temp, nullptr, 2);
        // append to message.
        message += (char)ascii_value;
    }
    return message;
}

int main(int argc, char const *argv[])
{
    int h3_socket;
    struct sockaddr_in h2_address, h3_address;
    // create a UDP socket (SOCK_DGRAM)
    if ((h3_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        cerr << "Could not create socket\n";
        exit(1);
    }
    memset(&h2_address, 0, sizeof(h2_address));
    memset(&h3_address, 0, sizeof(h3_address));

    // tell h3 where to listen.
    h3_address.sin_family = AF_INET;
    h3_address.sin_addr.s_addr = htonl(INADDR_ANY);
    // this is port to which h2 forwards the UDP packet.
    h3_address.sin_port = htons(9567);

    // bind the socket of h3 to the ip and port
    if (bind(h3_socket, (struct sockaddr *)&h3_address, sizeof(h3_address)) < 0)
    {
        cout << "Could not bind socket\n";
        exit(1);
    }
    cout << "Server is listening on port 9567\n";
    // stores the received code.
    char received_code[1024];
    string message = "";
    string prev_byte = "";
    bool isMessage = false;
    while (true)
    {
        // read a message from h2
        int size = recvfrom(h3_socket, received_code, 1024, 0, (struct sockaddr *)&h2_address, (socklen_t *)&h2_address);
        received_code[size] = '\0';
        // get the original data bits.
        string temp = retrieve_message(received_code);
        // If it is either a start or a ending flag bit.
        if (temp == FLAG)
        {
            isMessage = !isMessage;
            prev_byte = FLAG;
            // if this is a flag bit that occurs at the end,
            // Print the original message and reset the states.
            if (!isMessage)
            {
                cout << binary_to_string(message) << endl;
                message = "";
                FLAG_SET = false;
                FLAG = "1";
                ESC = "0";
            }
        }
        /** This represents the byte stuffing, 
         * Here there are two possibilities now
         *      1. Another ESC occurs after this
         *      2. A FLAG frame occurs after this
         * In an case, discard the current ESC and read the next
         * frame, decode it and store it in the message.
        */
        else if (temp == ESC)
        {
            int size = recvfrom(h3_socket, received_code, 1024, 0, (struct sockaddr *)&h2_address, (socklen_t *)&h2_address);
            received_code[size] = '\0';
            string temp = retrieve_message(received_code);
            message += temp;
        }
        else
        {
            // if it is none of the special bytes, store the frame
            // in the message variable.
            message += temp;
        }
    }
}