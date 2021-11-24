def read_public_key(username):
    with open(f'{username}.pub', 'r') as f:
        e, n = f.read().split()
        return int(e), int(n)


def convert_m_to_int(m):
    for x in m:
        yield int(format(ord(x), 'b'), 2)
    # bin_str = ''.join(format(ord(x), 'b') for x in m)
    # return int(bin_str, 2)


def encrypt(m, e, n):
    return pow(m, e, n)


if __name__ == '__main__':
    e, n = read_public_key('B')

    with open('message.txt', 'r') as f:
        m = f.read()
    print("Original message: " + str(m))
    with open('secret.txt', 'w') as f:
        for m_converted in convert_m_to_int(m):
            m_encrypted = encrypt(m_converted, e, n)
            f.write(str(m_encrypted) + '\n')
