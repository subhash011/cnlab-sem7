from utils import *
from hashlib import sha256


def convert_m_to_int(m):
    bin_int = int.from_bytes(m.encode(), 'big')
    bin_str = bin(bin_int)[2:]
    yield len(bin_str) % k
    for i in range(0, len(bin_str), k):
        yield int(bin_str[i:i+k].zfill(k), 2)


def convert_hash_to_int(hash):
    bin_int = int.from_bytes(hash.encode(), 'big')
    bin_str = bin(bin_int)[2:]
    return int(bin_str, 2)


def encrypt(m, e, n):
    return pow(m, e, n)


if __name__ == '__main__':
    e, n_b = read_public_key('B')
    d, n_a = read_private_key('A')

    with open('message.txt', 'r') as f:
        m = f.read()
    encrypted_m = ""
    global k
    k = get_k(n_b)
    for m_converted in convert_m_to_int(m):
        m_i = encrypt(m_converted, e, n_b)
        encrypted_m += str(m_i) + " "
    hsh = sha256(m.encode()).hexdigest()
    with open('secret.txt', 'w') as f:
        m_converted = convert_hash_to_int(hsh)
        m_i = encrypt(m_converted, d, n_a)
        f.write(encrypted_m + '\n' + str(m_i))
