def read_private_key(username):
    with open(f'{username}.pri', 'r') as f:
        d, n = f.read().split()
        return int(d), int(n)


def get_k(n):
    k = 0
    while True:
        if 2**k > n:
            return k - 1
        k += 1


def convert_m_to_str(m, n):
    binary_int = int(m, 2)
    byte_number = binary_int.bit_length() + 7 // 8
    bin_arr = binary_int.to_bytes(byte_number, 'big')
    return bin_arr.decode()


def decrypt(c, d, n):
    return pow(c, d, n)


if __name__ == '__main__':
    with open('secret.txt', 'r') as f:
        lines = f.readlines()
    d, n = read_private_key('B')
    m_decrypted = ""
    itr = enumerate(lines)
    _, last_block = next(itr)
    last_block_len = decrypt(int(last_block), d, n)
    for i, line in itr:
        decoded = decrypt(int(line), d, n)
        if i == len(lines) - 1:
            m_decrypted += bin(decoded)[2:].zfill(last_block_len)
            break
        m_decrypted += bin(decoded)[2:].zfill(get_k(n))
    print(f"Decrypted message: {convert_m_to_str(m_decrypted, n)}")
