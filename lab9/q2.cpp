#include "packet.h"
#include <dirent.h>

using namespace std;

/**
 * @brief This function adds two binary strings, if there is a carry,
 * it adds it back to the result and returns it.
 * @param b1: The first binary string
 * @param b2: The second binary string
 * @return: A binary string that is the result of adding the two binary strings
 * as described above.
 */
bitset<16> add(std::bitset<16> &b1, std::bitset<16> &b2)
{
    // add the two binary strings
    bitset<17> res(b1.to_ullong() + b2.to_ullong());
    // if there is a carry, add it back to the result
    if (res[16] == 1)
    {
        res = bitset<17>(res.to_ullong() + 1);
    }
    return bitset<16>(res.to_ullong());
}

int main(int argc, char const *argv[])
{
    struct dirent *entry;
    DIR *dir;
    // open the directory
    string dir_path = argc > 1 ? argv[1] : "./ipfrags";
    if ((dir = opendir(dir_path.c_str())) == NULL)
    {
        cerr << "Error opening directory" << endl;
        exit(EXIT_FAILURE);
    }
    vector<string> file_names{};
    // for each file in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // if it is a regular file
        if (entry->d_type == DT_REG)
        {
            // add it to the vector of file names
            string file_path = dir_path + "/" + entry->d_name;
            file_names.push_back(file_path);
        }
    }
    // close the directory
    closedir(dir);
    ofstream output_file(VALID_FRAGS_LOC);
    // for each file in the vector of file names
    for (auto &file_name : file_names)
    {
        IPPacket *fragment = new IPPacket();
        // open and read the file in binary mode
        ifstream fp(file_name, ios::binary);
        fp.read((char *)fragment, sizeof(IPPacket));
        // cast the packet to uint16_t for easy pointer arithmetic
        uint16_t *ptr = (uint16_t *)fragment;
        // find the header length of the fragment from v_hl
        int header_len = (fragment->v_hl & 0x0F) * 4;
        // stores the result of the checksum
        bitset<16> res(0);
        for (int i = 0; i < header_len / 2; i++)
        {
            bitset<16> b1(*ptr);
            // add the current 16 bit binary string to the result
            res = add(res, b1);
            ptr++;
        }
        // if all the bits in the result are set, the fragment is valid
        if (res.all())
        {
            cout << "File: " << file_name << endl;
            // print the header information
            cout << get_headers(*fragment) << endl;
            // write the file name to the output file for q3
            output_file << file_name << endl;
        }
    }
    return 0;
}