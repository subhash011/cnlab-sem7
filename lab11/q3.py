from utils import *


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


def decrypt(c, d, n): return pow(c, d, n)


def unsign(m, e, n): return pow(m, e, n)


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
        block = decrypt(int(block), d, n_b)
        # since each block was signed, we use the unsign function
        # to verify the signature
        decrypted_block = unsign(int(block), e, n_a)
        # since each block is pre and post padded with a 1, ignore it.
        decrypted_msg += bin(decrypted_block)[3:-1]

    # convert the binary string to a string of ASCII characters
    decrypted_msg = bin2str(decrypted_msg)
    if decrypted_msg is None:
        # if the decrypted message is None, then the message was
        # not properly decrypted or the signature was invalid
        print("Message not verfied!")
    else:
        print(decrypted_msg)
