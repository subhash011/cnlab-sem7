from random import randint


def is_prime(n: int) -> bool:
    if n <= 3:
        return n > 1
    if n % 2 == 0 or n % 3 == 0:
        return False
    i = 5
    while i ** 2 <= n:
        if n % i == 0 or n % (i + 2) == 0:
            return False
        i += 6
    return True


def gcd(a, b):
    while b != 0:
        a, b = b, a % b
    return a


def mod_inverse(a, m):
    a %= m
    for x in range(1, m):
        if (a * x) % m == 1:
            return x
    return None


def generate_rsa(p, q):
    n = p * q
    z = (p - 1) * (q - 1)
    d = 13
    while d < z:
        if gcd(d, z) == 1:
            break
        d += 1
    e = mod_inverse(d, z)
    return n, e, d


def generate_primes():
    p = randint(10, 100)
    q = randint(10, 100)
    while not is_prime(p):
        p += 1
    while not is_prime(q) and q != p:
        q += 1
    return p, q


def write_key_user(username):
    p, q = generate_primes()
    n, e, d = generate_rsa(p, q)
    print(f"User: {username}")
    print("Public key: (e, n) = ({}, {})".format(e, n))
    print("Private key: (d, n) = ({}, {})".format(d, n))
    with open(f'{username}.pub', "w") as f:
        f.write(str(e) + ' ' + str(n) + '\n')
    with open(f'{username}.pri', "w") as f:
        f.write(str(d) + ' ' + str(n) + '\n')


if __name__ == '__main__':
    write_key_user('A')
    write_key_user('B')
