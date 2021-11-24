def read_private_key(username):
    with open(f'{username}.pri', 'r') as f:
        d, n = f.read().split()
        return int(d), int(n)


def read_public_key(username):
    with open(f'{username}.pub', 'r') as f:
        e, n = f.read().split()
        return int(e), int(n)


def get_k(n):
    k = 0
    while True:
        if 2**k > n:
            return k - 1
        k += 1


def convert_m_to_int(m, n):
    k = get_k(n)
    bin_int = int.from_bytes(m.encode(), 'big')
    bin_str = bin(bin_int)[2:]
    yield len(bin_str) % k
    for i in range(0, len(bin_str), k):
        yield int(bin_str[i:i+k].zfill(k), 2)


def encrypt(m, e, n):
    return pow(m, e, n)


if __name__ == '__main__':
    e, n = read_public_key('B')

    with open('message.txt', 'r') as f:
        m = f.read()
    print("Original message: " + str(m))
    with open('secret.txt', 'w') as f:
        for m_converted in convert_m_to_int(m, n):
            m_encrypted = encrypt(m_converted, e, n)
            f.write(str(m_encrypted) + '\n')
