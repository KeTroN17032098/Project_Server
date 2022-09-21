#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define MAX_DIGITS 50
#define PRIME_SOURCE_FILE "primes.txt"
// This is the header file for the library librsaencrypt.a

// Change this line to the file you'd like to use as a source of primes.
// The format of the file should be one prime per line.


struct public_key_class {
	long long modulus;
	long long exponent;
};

struct private_key_class {
	long long modulus;
	long long exponent;
};

long long gcd(long long a, long long b);

long long ExtEuclid(long long a, long long b);
static long long modmult(long long a, long long b, long long mod);
long long rsa_modExp(long long b, long long e, long long m);
// This function generates public and private keys, then stores them in the structures you
// provide pointers to. The 3rd argument should be the text PRIME_SOURCE_FILE to have it use
// the location specified above in this header.
void rsa_gen_keys(struct public_key_class* pub, struct private_key_class* priv);

// This function will encrypt the data pointed to by message. It returns a pointer to a heap
// array containing the encrypted data, or NULL upon failure. This pointer should be freed when 
// you are finished. The encrypted data will be 8 times as large as the original data.
long long* rsa_encrypt(const char* message, const unsigned long message_size, const struct public_key_class* pub);

// This function will decrypt the data pointed to by message. It returns a pointer to a heap
// array containing the decrypted data, or NULL upon failure. This pointer should be freed when 
// you are finished. The variable message_size is the size in bytes of the encrypted message. 
// The decrypted data will be 1/8th the size of the encrypted data.
char* rsa_decrypt(const long long* message, const unsigned long message_size, const struct private_key_class* pub);
