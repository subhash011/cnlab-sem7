from utils import *
from hashlib import sha1


def bin2str(s):
    """
    Convert a binary string to a string of ASCII characters.
    """
    try:
        binary_int = int(s, 2)
        byte_number = binary_int.bit_length() + 7 // 8
        # convert the integer to a byte string
        bin_arr = binary_int.to_bytes(byte_number, 'big')
        # decode the byte string to a string of ASCII characters
        ret_str = bin_arr.decode()
        # remove leading and trailing null characters
        ret_str = ret_str.strip('\x00')
        return ret_str
    except:
        return None


def decrypt(c, d, n):
    return pow(c, d, n)


if __name__ == '__main__':

    # get private key of B for decrypting the message
    d, n_b = read_private_key('B')
    # get the public key of A for verifying the signature
    e, n_a = read_public_key('A')

    with open('secret.txt', 'r') as f:
        lines = f.readlines()

    decrypted_msg = ""
    encrypted_m = lines[0].split()
    for i, block in enumerate(encrypted_m):
        # for each encrypted block, decrypt it
        decrypted_block = decrypt(int(block), d, n_b)
        # since each block is pre and post padded with a 1, ignore it.
        decrypted_msg += bin(decrypted_block)[3:-1]

    # convert the binary string to a string of ASCII characters
    decrypted_msg = bin2str(decrypted_msg)
    # if the message was tampered with, the decrypted message will be None
    if decrypted_msg is None:
        print("Decryption failed")
        exit(1)
    # calculate the sha-1 hash of the decrypted message
    hsh = sha1(decrypted_msg.encode()).hexdigest()
    # decrypt to get the message digest from A
    md = decrypt(int(lines[1]), e, n_a)
    decrypted_hsh = bin2str(bin(md)[2:])
    # verify the signature i.e if the hash of the decrypted message
    # is the same as the decrypted hash from A
    if (hsh == decrypted_hsh):
        print(decrypted_msg)
    else:
        print("Message not verified!")
