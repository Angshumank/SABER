#ifndef INDCPA_H
#define INDCPA_H

#include"cpucycles.h"
#include"poly.h"

uint64_t clock_sujoy1, clock_sujoy2, clock_samp, clock_arith, clock_load;

void indcpa_keypair(unsigned char *pk, unsigned char *sk);

void sample_matrix(polyvec *a, const unsigned char *seed);

void indcpa_client(unsigned char *pk, unsigned char *b_prime, unsigned char *c, unsigned char *key);

void indcpa_server(unsigned char *pk, unsigned char *b_prime, unsigned char *c, unsigned char *key);

void indcpa_kem_keypair(unsigned char *pk, unsigned char *sk);
void indcpa_kem_enc(unsigned char *message, unsigned char *noiseseed, const unsigned char *pk, unsigned char *ciphertext);
void indcpa_kem_dec(const unsigned char *sk, const unsigned char *ciphertext, unsigned char *message_dec);

uint64_t clock1,clock2;

#endif

