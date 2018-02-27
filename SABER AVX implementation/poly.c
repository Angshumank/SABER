/*---------------------------------------------------------------------
This file has been adapted from the implementation 
(available at, Public Domain https://github.com/pq-crystals/kyber) 
of "CRYSTALS – Kyber: a CCA-secure module-lattice-based KEM"
by : Joppe Bos, Leo Ducas, Eike Kiltz, Tancrede Lepoint, 
Vadim Lyubashevsky, John M. Schanck, Peter Schwabe & Damien stehle
----------------------------------------------------------------------*/

#include <stdio.h>
#include "poly.h"
#include "cbd.h"
#include "fips202.h"
#include "fips202x4.h"
#include "crypto_stream.h"

void poly_getnoise(uint16_t *r,const unsigned char *seed, unsigned char nonce)
{
  unsigned char buf[LWR_N];

  cshake128_simple(buf,LWR_N,nonce,seed,LWR_NOISESEEDBYTES);

  cbd( r, buf);
}


void poly_getnoise4x(uint16_t *r0, uint16_t *r1, uint16_t *r2, const unsigned char *seed, unsigned char nonce0, unsigned char nonce1, unsigned char nonce2, unsigned char nonce3)
{
  uint16_t nblocks=2;
  unsigned char buf0[SHAKE128_RATE*nblocks];
  unsigned char buf1[SHAKE128_RATE*nblocks];
  unsigned char buf2[SHAKE128_RATE*nblocks];
  unsigned char buf3[SHAKE128_RATE*nblocks];

  cshake128_simple4x(buf0,buf1,buf2,buf3,SHAKE128_RATE*nblocks,nonce0,nonce1,nonce2,nonce3,seed,LWR_NOISESEEDBYTES);

  	
  cbd( r0, buf0);
  cbd( r1, buf1);
  cbd( r2, buf2);
}


/*
void poly_getnoise4x(poly *r0, poly *r1, poly *r2, poly *r3, const unsigned char *seed, unsigned char nonce0, unsigned char nonce1, unsigned char nonce2, unsigned char nonce3)
{
  unsigned char buf0[LWR_N];
  unsigned char buf1[LWR_N];
  unsigned char buf2[LWR_N];
  unsigned char buf3[LWR_N];

  cshake128_simple4x(buf0,buf1,buf2,buf3,LWR_N,nonce0,nonce1,nonce2,nonce3,seed,LWR_NOISESEEDBYTES);

  cbd((uint16_t *) r0, buf0);
  cbd((uint16_t *) r1, buf1);
  cbd((uint16_t *) r2, buf2);
  cbd((uint16_t *) r3, buf3);
}
*/


