from utils import *
from utils import generate_prime
from math import gcd

k = 512


def extended_gcd(a, b):
    """
    This is an extended euclidean algorithm.
    For more information, see https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
    """
    old_r, r = a, b
    old_s, s = 1, 0
    old_t, t = 0, 1
    while r != 0:
        quotient = old_r // r
        old_r, r = r, old_r - quotient * r
        old_s, s = s, old_s - quotient * s
        old_t, t = t, old_t - quotient * t
    return old_r, old_s, old_t


def modular_inverse(a, m):
    """
    Calculates the modular inverse of a modulo m.
    """
    # return pow(a, -1, m)
    g, x, _ = extended_gcd(a, m)
    if g != 1:
        return None
    return x % m


def generate_rsa(p, q):
    n = p * q
    z = (p - 1) * (q - 1)
    d = 13
    while d < z:
        if gcd(d, z) == 1:
            break
        d += 1
    e = modular_inverse(d, z)
    return n, e, d


def generate_primes():
    p = generate_prime(k//2)
    q = generate_prime(k//2)
    return p, q


def write_key_user(username):
    p, q = generate_primes()
    n, e, d = generate_rsa(p, q)
    print(f"User: {username}")
    print("Public key: (e, n) = ({}, {})".format(e, n))
    print("Private key: (d, n) = ({}, {})".format(d, n))
    with open(f'{username}.pub', "w") as f:
        f.write(f"{e} {n}")
    with open(f'{username}.pri', "w") as f:
        f.write(f"{d} {n}")


if __name__ == '__main__':
    write_key_user('A')
    write_key_user('B')
