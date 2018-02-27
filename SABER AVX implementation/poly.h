/*---------------------------------------------------------------------
This file has been adapted from the implementation 
(available at, Public Domain https://github.com/pq-crystals/kyber) 
of "CRYSTALS – Kyber: a CCA-secure module-lattice-based KEM"
by : Joppe Bos, Leo Ducas, Eike Kiltz, Tancrede Lepoint, 
Vadim Lyubashevsky, John M. Schanck, Peter Schwabe & Damien stehle
----------------------------------------------------------------------*/

#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "LWR_params.h"

uint64_t clock_mul, clock_matrix, clock_secret,count_mul;

typedef struct
{
  uint16_t coeffs[LWR_N];
} poly;

typedef struct{
  poly vec[LWR_K];
} polyvec;

void poly_getnoise(uint16_t *r,const unsigned char *seed, unsigned char nonce);

void poly_getnoise4x(uint16_t *r0, uint16_t *r1, uint16_t *r2, const unsigned char *seed, unsigned char nonce0, unsigned char nonce1, unsigned char nonce2, unsigned char nonce3);

//void poly_getnoise4x(poly *r0, poly *r1, poly *r2, poly *r3, const unsigned char *seed, unsigned char nonce0, unsigned char nonce1, unsigned char nonce2, unsigned char nonce3);

#endif