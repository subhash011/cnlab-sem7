from random import getrandbits, randint


def get_k(n):
    """
    Finds the max block size for encryption.
    It finds the max k such that 2^k < n where n is the modulus
    of the RSA encryption.
    """
    k = 0
    while True:
        if 2**k > n:
            return k - 1
        k += 1


def read_public_key(username):
    """Read the public key of a user"""
    with open(f'{username}.pub', 'r') as f:
        e, n = f.read().split()
        return int(e), int(n)


def read_private_key(username):
    """Read the private key of a user"""
    with open(f'{username}.pri', 'r') as f:
        d, n = f.read().split()
        return int(d), int(n)


def is_prime(n, k=256):
    """
    Uses the Miller-Rabin primality test to determine if a number is prime.\n
    Referenced from https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
    """
    if n == 2:
        return True
    if n <= 1 or n & 1 == 0:
        return False
    s = 0
    d = n - 1
    while d & 1 == 0:
        s += 1
        d //= 2
    for _ in range(k):
        a = randint(2, n - 1)
        x = pow(a, d, n)
        if x == 1 or x == n - 1:
            continue
        for _ in range(1, s):
            x = pow(x, 2, n)
            if x == 1:
                return False
        return False
    return True


def generate_prime(n_bits=512):
    """
    Generates a prime number of n_bits bits.
    """
    p = 1
    # until p is a prime number
    while not is_prime(p):
        # generate a random number of n_bits bits
        p = getrandbits(n_bits)
        # make sure it's odd and make the first bit 1 so that it
        # does not have leading zeros
        p |= (1 << n_bits - 1) | 1
    return p
