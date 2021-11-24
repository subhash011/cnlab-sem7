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
    g, x, _ = extended_gcd(a, m)
    if g != 1:
        return None
    return x % m


def generate_rsa(p, q):
    """
    Generates a key pair, given two primes p and q.
    Note: 
    """
    # the modulus n is the product of p and q
    n = p * q
    # the totient of n is (p-1)*(q-1)
    totient = (p - 1) * (q - 1)
    # the public exponent (used in encryption)
    e = 5
    while e < totient:
        # find a "e" that is relatively prime to z
        if gcd(e, totient) == 1:
            break
        e += 1
    # d is the secret exponent (used in decryption)
    d = modular_inverse(e, totient)
    return n, e, d


def generate_primes():
    """
    Returns two primes p and q of k/2 bits each.
    """
    # generate two primes of length k/2 bits each.
    p = generate_prime(k//2)
    q = generate_prime(k//2)
    return p, q


def write_key_user(username):
    """
    Given a username, it writes the two key pairs to their
    respective files.
    """
    # generate two large primes
    p, q = generate_primes()
    # generate parameters for RSA
    n, e, d = generate_rsa(p, q)
    with open(f'{username}.pub', "w") as f:
        f.write(f"{e} {n}")
    with open(f'{username}.pri', "w") as f:
        f.write(f"{d} {n}")
    print(f"Private and public keys for {username} successfully generated.")


if __name__ == '__main__':
    # write keys for A to A.pub and A.pri
    write_key_user('A')
    # write keys for B to B.pub and B.pri
    write_key_user('B')
