from utils import *
from hashlib import sha1


def msg2blocks(m):
    """
    Since the message can be of arbitrary length, we need to split it into blocks
    of size k such that 2^k is less than the modulus n.
    """
    bin_int = int.from_bytes(m.encode(), 'big')
    bin_str = bin(bin_int)[2:]
    for i in range(0, len(bin_str), k - 2):
        # pre and post pad the block with 1 so that we don't lose any bits
        # even when there are leading zeros
        yield int('1' + bin_str[i: i + k - 2] + '1', 2)


def hash2int(hash):
    bin_int = int.from_bytes(hash.encode(), 'big')
    bin_str = bin(bin_int)[2:]
    return int(bin_str, 2)


def encrypt(m, e, n):
    return pow(m, e, n)


def sign(m, d, n):
    return pow(m, d, n)


if __name__ == '__main__':
    global k
    # get the public key of B for encrypting the message
    e, n_b = read_public_key('B')
    k = get_k(n_b)
    # get the private key of A for signing the message
    d, n_a = read_private_key('A')

    # get the message
    with open('message.txt', 'r') as f:
        msg = f.read()

    encrypted_msg = ""
    for block in msg2blocks(msg):
        # encrypt the block
        encrypted_block = encrypt(block, e, n_b)
        # append the encrypted block to the encrypted message
        encrypted_msg += str(encrypted_block) + " "

    # calculate sha-1 hash of the message
    msg_digest = sha1(msg.encode()).hexdigest()
    # convert the hash to an integer
    msg_digest = hash2int(msg_digest)
    # sign the message digest
    signature = sign(msg_digest, d, n_a)
    with open('secret.txt', 'w') as f:
        # write the encrypted message and the signature to secret.txt
        f.write(encrypted_msg + '\n' + str(signature))
    print("Encrypted message and signature written to secret.txt")
