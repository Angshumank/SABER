#include <immintrin.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "LWR_indcpa.h"
#include "LWR_indcca.h"
#include "verify.h"
#include "randombytes.h"
#include "LWR_params.h"
#include "fips202.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
  size_t i;
 
  indcpa_kem_keypair(pk, sk);						  // sk[0:LWR_INDCPA_SECRETKEYBYTES-1] <-- sk
  for(i=0;i<LWR_INDCPA_PUBLICKEYBYTES;i++)
    sk[i+LWR_INDCPA_SECRETKEYBYTES] = pk[i];				  // sk[LWR_INDCPA_SECRETKEYBYTES:LWR_INDCPA_SECRETKEYBYTES+LWR_INDCPA_SECRETKEYBYTES-1] <-- pk	
  shake128(sk+LWR_SECRETKEYBYTES-64,32, pk, LWR_INDCPA_PUBLICKEYBYTES);	  // Then hash(pk) is appended.	
  randombytes(sk+LWR_SECRETKEYBYTES-LWR_KEYBYTES , LWR_KEYBYTES );        // Remaining part of sk contains a pseudo-random number. This is output when check in crypto_kem_dec() fails. 
  return(0);	
}

int crypto_kem_enc(unsigned char *c, unsigned char *k, const unsigned char *pk)
{
  unsigned char krq[96];                             	  // Will contain key, coins, qrom-hash
  unsigned char buf[64];                          
  int i;

  randombytes(buf, 32);
  shake128(buf,32,buf,32);            			  //BUF[0:31] <-- random message (will be used as the key for client) Note: hash doesnot release system RNG output

  shake128(buf+32, 32, pk, LWR_INDCPA_PUBLICKEYBYTES);    // BUF[32:63] <-- Hash(public key);  Multitarget countermeasure for coins + contributory KEM 
  shake128(krq, 96, buf, 64);				  // krq[0:95] <-- Hash(buf[0:63]);  	
							  // K^ <-- krq[0:31]
							  // noiseseed (r) <-- krq[32:63];	
							  // d <-- krq[64:95];

  indcpa_kem_enc(buf, krq+32, pk,  c);



  for(i=0;i<32;i++)
    c[i+LWR_POLYVECCOMPRESSEDBYTES+LWR_RECONBYTES_KEM] = krq[i+64];

  shake128(krq+32, 32, c,LWR_POLYVECCOMPRESSEDBYTES + LWR_RECONBYTES_KEM + LWR_HASHBYTES);              // overwrite coins in krq with h(c)
  shake128(k, 32, krq, 64);                          					// hash concatenation of pre-k and h(c) to k 
  return(0);	
}


int crypto_kem_dec(unsigned char *k, const unsigned char *c, const unsigned char *sk)
{
  int i, fail;
  unsigned char cmp[LWR_BYTES_CCA_DEC];
  unsigned char buf[64];
  unsigned char krq[96];                             // Will contain key, coins, qrom-hash
  const unsigned char *pk = sk + LWR_INDCPA_SECRETKEYBYTES;

   indcpa_kem_dec(sk, c, buf);			     // buf[0:31] <-- message

 
  // Multitarget countermeasure for coins + contributory KEM 
  for(i=0;i<32;i++)                                  // Save hash by storing h(pk) in sk 
    buf[32+i] = sk[LWR_SECRETKEYBYTES-64+i]; 
  shake128(krq, 96, buf, 64);

   indcpa_kem_enc(buf, krq+32, pk, cmp);

  for(i=0;i<32;i++)
    cmp[i + LWR_POLYVECCOMPRESSEDBYTES + LWR_RECONBYTES_KEM] = krq[i+64];

  fail = verify(c, cmp, LWR_BYTES_CCA_DEC);


  shake128(krq+32, 32, c, LWR_BYTES_CCA_DEC);        		     // overwrite coins in krq with h(c) 

  cmov(krq, sk+LWR_SECRETKEYBYTES-LWR_KEYBYTES, LWR_KEYBYTES, fail); 
  shake128(k, 32, krq, 64);                          	   	     // hash concatenation of pre-k and h(c) to k
  return(0);	
}






