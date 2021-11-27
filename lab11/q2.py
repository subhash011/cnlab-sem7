from utils import *


def str2int(s):
    bin_int = int.from_bytes(s.encode(), 'big')
    bin_str = bin(bin_int)[2:]
    return int(bin_str, 2)


def msg2blocks(m):
    """
    Since the message can be of arbitrary length, we need to split it into blocks
    s.t each block is smaller than the modulus size.
    """
    n_bytes = modulus_size // 16
    for i in range(0, len(m), n_bytes):
        yield str2int(m[i:i+n_bytes])


def encrypt(m, e, n): return pow(m, e, n)


# this function is similar to the decrypt function, it has
# been renamed for clarity
def sign(m, d, n): return pow(m, d, n)


if __name__ == '__main__':
    # get the public key of B for encrypting the message
    e, n_b = read_public_key('B')
    # get the private key of A for signing the message
    d, n_a = read_private_key('A')

    # get the message
    with open('message.txt', 'r') as f:
        msg = f.read()
    # append the public key of B to the message
    # this allows B to verify the signature
    msg += ":" + str((e, n_b))
    encrypted_msg = ""
    for block in msg2blocks(msg):
        # sign the block
        block = sign(block, d, n_a)
        # encrypt the block
        encrypted_block = encrypt(block, e, n_b)
        # append the encrypted block to the encrypted message
        encrypted_msg += str(encrypted_block) + " "
    with open('secret.txt', 'w') as f:
        # write the encrypted message and the signature to secret.txt
        f.write(encrypted_msg)
    print("Signed and encrypted message written to secret.txt")
