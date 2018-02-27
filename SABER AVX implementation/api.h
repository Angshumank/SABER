//
//  api.h
//
//  Created by Bassham, Lawrence E (Fed) on 9/6/17.
//  Copyright © 2017 Bassham, Lawrence E (Fed). All rights reserved.
//


//   This is a sample 'api.h' for use 'sign.c'

#ifndef api_h
#define api_h

//  Set these three values apropriately for your algorithm
#define CRYPTO_SECRETKEYBYTES 2304
#define CRYPTO_PUBLICKEYBYTES (3*320+32)
#define CRYPTO_BYTES 32
#define CRYPTO_CIPHERTEXTBYTES 1088
#define Saber_type 2
#define CRYPTO_ALGNAME "Saber"

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);
int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);

#endif /* api_h */
