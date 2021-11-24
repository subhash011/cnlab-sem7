from utils import *
from hashlib import sha256


def convert_m_to_str(m):
    binary_int = int(m, 2)
    byte_number = binary_int.bit_length() + 7 // 8
    bin_arr = binary_int.to_bytes(byte_number, 'big')
    ret_str = bin_arr.decode()
    ret_str = ret_str.strip('\x00')
    return ret_str


def decrypt(c, d, n):
    return pow(c, d, n)


if __name__ == '__main__':
    with open('secret.txt', 'r') as f:
        lines = f.read()
    d, n_b = read_private_key('B')


if __name__ == '__main__':
    with open('secret.txt', 'r') as f:
        lines = f.readlines()
    d, n_b = read_private_key('B')
    e, n_a = read_public_key('A')
    decrypted_m = ""
    encrypted_m = lines[0].split()
    itr = enumerate(encrypted_m)
    _, last_block = next(itr)
    last_block_len = decrypt(int(last_block), d, n_b)
    k = get_k(n_b)
    for i, line in itr:
        decoded = decrypt(int(line), d, n_b)
        if i == len(encrypted_m) - 1:
            decrypted_m += bin(decoded)[2:].zfill(last_block_len)
            break
        decrypted_m += bin(decoded)[2:].zfill(k)
    decrypted_m = convert_m_to_str(decrypted_m)
    hsh = sha256(decrypted_m.encode()).hexdigest()
    md = decrypt(int(lines[1]), e, n_a)
    decrypted_hsh = convert_m_to_str(bin(md)[2:])
    if (hsh == decrypted_hsh):
        print("Original message: " + decrypted_m)
    else:
        print("Message not verified!")
