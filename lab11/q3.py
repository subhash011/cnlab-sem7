def read_private_key(username):
    with open(f'{username}.pri', 'r') as f:
        d, n = f.read().split()
        return int(d), int(n)


def convert_m_to_str(m):
    m = str(hex(m))[2:]
    res = ""

    x = 0
    while x < len(m)-1:
        cur_hex_val = m[x] + m[x+1]
        cur_hex_val = int(cur_hex_val, 16)
        res += chr(cur_hex_val)
        x += 2
    return res


def decrypt(c, d, n):
    return pow(c, d, n)


if __name__ == '__main__':
    with open('secret.txt', 'r') as f:
        lines = f.readlines()
    d, n = read_private_key('B')
    m_decrypted = ""
    for m_encrypted in lines:
        decoded = decrypt(int(m_encrypted), d, n)
        m_decrypted += convert_m_to_str(decoded)
    print("Decrypted message: " + m_decrypted)
