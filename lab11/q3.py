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


# this function is similar to the encrypt function, it has
# been renamed for clarity
def unsign(m, e, n): return pow(m, e, n)


if __name__ == '__main__':

    # get private key of B for decrypting the message
    d_b, n_b = read_private_key('B')
    e_b, _ = read_public_key('B')
    # get the public key of A for verifying the signature
    e_a, n_a = read_public_key('A')

    with open('secret.txt', 'r') as f:
        lines = f.readlines()

    decrypted_msg = ""
    encrypted_m = lines[0].split()
    for block in encrypted_m:
        # for each encrypted block, decrypt it
        block = decrypt(int(block), d_b, n_b)
        # since each block was signed, we use the unsign function
        # to verify the signature
        decrypted_block = unsign(int(block), e_a, n_a)
        msg_str = bin2str(bin(decrypted_block)[2:])
        # if there is a failure while decoding, the message has been
        # tampered with
        if msg_str is None:
            print("Error: The message or the signature has been tampered!")
        decrypted_msg += msg_str
    # now this message contains two parts, the first part is the
    # original message, the second part is the signature i.e. the
    # public key of B in encrypted form
    # For example:
    #     If message.txt contained the message "foo:bar"
    #     A encrypts and sends "foo:bar:(e, n_b)"
    #     now splitting the message by the last ":" will give us
    #    "foo:bar" and "(e, n_b)" i.e. the message and the public key
    encrypted_key = decrypted_msg.rpartition(':')[-1]
    # if the public key in the message isn't the same as the public key of B
    # then the signature is invalid, so print the error message.
    if str((e_b, n_b)) != encrypted_key:
        print("Message not verified!")
        exit(1)
    message = decrypted_msg.rpartition(':')[0]
    print(message)
