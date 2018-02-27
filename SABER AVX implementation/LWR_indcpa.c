#include <stdint.h>
#include <stdio.h>

#include "LWR_indcpa.h"
//#include "poly.h"
#include "pack_unpack.h"
#include "randombytes.h"
#include "recon.h"
#include "cbd.h"
#include "./polymul/toom_cook_4/toom-cook_4way.c"
#include "LWR_params.h"

#include <immintrin.h>
#include <string.h>

//#include "cpucycles.h"
#include "fips202.h"
#include "fips202x4.h"


	uint64_t mask_ar[4]={~(0UL)};
	__m256i mask_load;
	__m256i floor_round;




/*--------------------------------------------------------------------------------------
	This routine loads the constant values for Toom-Cook multiplication 
----------------------------------------------------------------------------------------*/
void load_values(){

	int64_t i;

	int64_t inv3=43691;
	int64_t inv9=36409;
	int64_t inv15=61167;

	int64_t int45=45;
	int64_t int30=30;
	int64_t int0=0;


	int16_t inv3_avx_load[16],inv9_avx_load[16],inv15_avx_load[16],int45_avx_load[16],int30_avx_load[16],int0_avx_load[16];

	for(i=0;i<16;i++){
		inv3_avx_load[i]=inv3;
		inv9_avx_load[i]=inv9;
		inv15_avx_load[i]=inv15;
		int45_avx_load[i]=int45;
		int30_avx_load[i]=int30;
		int0_avx_load[i]=int0;
	}

	inv3_avx = _mm256_loadu_si256 ((__m256i const *) (&inv3_avx_load));
	inv9_avx = _mm256_loadu_si256 ((__m256i const *) (&inv9_avx_load));
	inv15_avx = _mm256_loadu_si256 ((__m256i const *) (&inv15_avx_load));
	int45_avx = _mm256_loadu_si256 ((__m256i const *) (&int45_avx_load));
	int30_avx = _mm256_loadu_si256 ((__m256i const *) (&int30_avx_load));
	int0_avx = _mm256_loadu_si256 ((__m256i const *) (&int0_avx_load));
	mask = _mm256_loadu_si256 ((__m256i const *)mask_ar);	
}



/*-----------------------------------------------------------------------------------
	This routine generates a=[Matrix K x K] of 256-coefficient polynomials 
-------------------------------------------------------------------------------------*/

void sample_matrix_old(polyvec *a, const unsigned char *seed)
{
  uint32_t nblocks=24;
  unsigned char buf[SHAKE128_RATE*nblocks];
  int i;


  // This creates a random byte bank of size SHAKE128_RATE*nblocks
  cshake128_simple4x(buf, buf + SHAKE128_RATE*6, buf + SHAKE128_RATE*12, buf + SHAKE128_RATE*18,SHAKE128_RATE*6,
                       0x0000, 0x0100, 0x0200, 0x0001,
                       seed,LWR_SEEDBYTES);


  for(i=0;i<LWR_K;i++)
  {
    LWR_un_pack13bit(buf+i*1248, (uint16_t (*)[LWR_N])(&a[i]) );    // stream 1248 i.e 3*256*13/8 bytes in LWR_K polynomials each having LWR_N coefficients. Note [a] is a matrix of LWR_K*LWR_K polynomials
  }
}

void BS2POLq(const unsigned char *bytes, uint16_t data[LWR_N]){
	
	uint32_t j;
	uint32_t offset_data=0,offset_byte=0;	
	
	offset_byte=0;

		for(j=0;j<LWR_N/8;j++){
			offset_byte=13*j;
			offset_data=8*j;
			data[offset_data + 0]= ( bytes[ offset_byte + 0 ] & (0xff)) | ((bytes[offset_byte + 1] & 0x1f)<<8);
			data[offset_data + 1]= ( bytes[ offset_byte + 1 ]>>5 & (0x07)) | ((bytes[offset_byte + 2] & 0xff)<<3) | ((bytes[offset_byte + 3] & 0x03)<<11);
			data[offset_data + 2]= ( bytes[ offset_byte + 3 ]>>2 & (0x3f)) | ((bytes[offset_byte + 4] & 0x7f)<<6);
			data[offset_data + 3]= ( bytes[ offset_byte + 4 ]>>7 & (0x01)) | ((bytes[offset_byte + 5] & 0xff)<<1) | ((bytes[offset_byte + 6] & 0x0f)<<9);
			data[offset_data + 4]= ( bytes[ offset_byte + 6 ]>>4 & (0x0f)) | ((bytes[offset_byte + 7] & 0xff)<<4) | ((bytes[offset_byte + 8] & 0x01)<<12);
			data[offset_data + 5]= ( bytes[ offset_byte + 8]>>1 & (0x7f)) | ((bytes[offset_byte + 9] & 0x3f)<<7);
			data[offset_data + 6]= ( bytes[ offset_byte + 9]>>6 & (0x03)) | ((bytes[offset_byte + 10] & 0xff)<<2) | ((bytes[offset_byte + 11] & 0x07)<<10);
			data[offset_data + 7]= ( bytes[ offset_byte + 11]>>3 & (0x1f)) | ((bytes[offset_byte + 12] & 0xff)<<5);
		}


}


void sample_matrix(polyvec *a, const unsigned char *seed) 
{
  unsigned int one_vector=13*LWR_N/8;
  unsigned int byte_bank_length=LWR_K*LWR_K*one_vector;
  unsigned char buf[byte_bank_length];

  uint16_t temp_ar[LWR_N];

  int i,j,k;
  uint16_t mod = (LWR_Q-1);

  shake128(buf,byte_bank_length,seed,LWR_SEEDBYTES);
  
  for(i=0;i<LWR_K;i++)
  {
    for(j=0;j<LWR_K;j++)
    {
	BS2POLq(buf+(i*LWR_K+j)*one_vector,temp_ar);
	for(k=0;k<LWR_N;k++){
		a[i].vec[j].coeffs[k] = (temp_ar[k])& mod ;
	}
    }
  }
}

void GenSecret(uint16_t r[LWR_K][LWR_N],const unsigned char *seed){


		uint32_t i;

		int32_t buf_size= LWR_MU*LWR_N*LWR_K/8;

		uint8_t buf[buf_size];

		shake128(buf, buf_size, seed,LWR_NOISESEEDBYTES);

		for(i=0;i<LWR_K;i++)
		{
			cbd(r[i],buf+i*LWR_MU*LWR_N/8);
		}
}

void indcpa_kem_keypair(unsigned char *pk, unsigned char *sk)
{
 
  //clock_sujoy1 = cpucycles();

  polyvec a[LWR_K];

  uint16_t skpv1[LWR_K][LWR_N];

  
 
  unsigned char seed[LWR_SEEDBYTES];
  unsigned char noiseseed[LWR_COINBYTES];
  int32_t i,j,k;


//--------------AVX declaration------------------
	
  __m256i sk_avx[LWR_K][LWR_N/16];
  __m256i mod;
  __m256i res_avx[LWR_K][LWR_N/16];
  __m256i a_avx[LWR_K][LWR_K][LWR_N/16];
  __m256i acc[2*LWR_N/16];

  mask_ar[0]=~(0UL);mask_ar[1]=~(0UL);mask_ar[2]=~(0UL);mask_ar[3]=~(0UL);
  mask_load = _mm256_loadu_si256 ((__m256i const *)mask_ar);

  mod=_mm256_set1_epi16(LWR_Q-1);
  floor_round=_mm256_set1_epi16(4);


//--------------AVX declaration ends------------------

   load_values();


  randombytes(seed, LWR_SEEDBYTES);
 


  shake128(seed, LWR_SEEDBYTES, seed, LWR_SEEDBYTES); // for not revealing system RNG state
  randombytes(noiseseed, LWR_COINBYTES);

  clock1=cpucycles();
  sample_matrix(a, seed); //sample matrix A
  clock2=cpucycles();
  clock_matrix=clock_matrix+(clock2-clock1);


  //clock_sujoy2 = cpucycles();	
  //clock_load = clock_load + clock_sujoy2 - clock_sujoy1;

  
  //clock_sujoy1 = cpucycles();

 //poly_getnoise4x(skpv1[0], skpv1[1], skpv1[2], noiseseed, 0x0001, 0x0002, 0x0003, 0x0004);
 clock1=cpucycles();
 GenSecret(skpv1,noiseseed);
 clock2=cpucycles();
 clock_secret=clock_secret+(clock2-clock1);

		
 for(i=0;i<LWR_K;i++)
 {
	//poly_getnoise(skpv1[i],noiseseed,nonce++); 	//generate secret from constant-time binomial distribution
	for(j=0; j<LWR_N/16; j++){
            sk_avx[i][j] = _mm256_loadu_si256 ((__m256i const *) (&skpv1[i][j*16]));
	}

  }

  //clock_sujoy2 = cpucycles();	
  //clock_samp = clock_samp + clock_sujoy2 - clock_sujoy1;

 //clock_sujoy1 = cpucycles();	

  for(i=0;i<LWR_K;i++){ //load the vector a in avx
	  for(j=0;j<LWR_K;j++){
		  for(k=0;k<LWR_N/16;k++){
			a_avx[i][j][k]=_mm256_loadu_si256 ((__m256i const *) (&a[i].vec[j].coeffs[k*16]));
		  }
	  }
  }	



  //------------------------do the matrix vector multiplication and rounding------------


	for(i=0;i<LWR_K;i++){
		for(j=0;j<LWR_N/16;j++){
			res_avx[i][j]=_mm256_xor_si256(res_avx[i][j],res_avx[i][j]);
		}
	}

	for(i=0;i<LWR_K;i++){
		for(j=0;j<LWR_K;j++){

			toom_cook_4way_avx(a_avx[i][j], sk_avx[j], LWR_Q, acc);


			for(k=0;k<LWR_N/16;k++){
				res_avx[i][k]=_mm256_add_epi16(res_avx[i][k],acc[k]);
				res_avx[i][k]=_mm256_and_si256(res_avx[i][k],mod); //reduction
				acc[k]=_mm256_xor_si256(acc[k],acc[k]); //clear the accumulator
			}
			
		}
	}

	
	  //-----now rounding

	for(i=0;i<LWR_K;i++){ //shift right 3 bits
		for(j=0;j<LWR_N/16;j++){
			res_avx[i][j]=_mm256_add_epi16 (res_avx[i][j], floor_round);
			res_avx[i][j]=_mm256_srli_epi16 (res_avx[i][j], 3);
			res_avx[i][j]=_mm256_and_si256 (res_avx[i][j], mod);			
		}
	}

	//------------------unload and pack sk=3 x (256 coefficients of 14 bits)-------
		
		LWR_pack13bit(sk,skpv1);

	//------------------unload and pack pk=256 bits seed and 3 x (256 coefficients of 11 bits)-------
	
	for(i=0;i<LWR_K;i++){ // first store in 16 bit arrays
		  for(j=0;j<LWR_N/16;j++){
		     _mm256_maskstore_epi32 ((int *) (skpv1[i]+j*16), mask_load, res_avx[i][j]);
		    //		    _mm256_maskstore_epi32 (skpv1[i]+j*16, mask_load, res_avx[i][j]);
					    }
	  }

	LWR_pack10bit(pk,skpv1); // load the public-key coefficients

	for(i=0;i<LWR_SEEDBYTES;i++){ // now load the seedbytes in PK. Easy since seed bytes are kept in byte format.
		pk[LWR_POLYVECCOMPRESSEDBYTES + i]=seed[i]; 
	}

 //clock_sujoy2 = cpucycles();	
 //clock_arith = clock_arith + clock_sujoy2 - clock_sujoy1;

}


void indcpa_kem_enc(unsigned char *message_received, unsigned char *noiseseed, const unsigned char *pk, unsigned char *ciphertext)
{ 

	clock_sujoy1 = cpucycles();

	uint32_t i,j,k;
	polyvec a[LWR_K];		// skpv;
	unsigned char seed[LWR_SEEDBYTES];
	uint16_t pkcl[LWR_K][LWR_N]; 	//public key of received by the client


	uint16_t skpv1[LWR_K][LWR_N];
	uint16_t temp[3][LWR_N];
	uint16_t message[LWR_KEYBYTES*8];

	unsigned char rec_c[LWR_RECONBYTES_KEM];
	//--------------AVX declaration------------------
	
	  __m256i sk_avx[LWR_K][LWR_N/16];
	  __m256i mod, mod_p;
	  __m256i res_avx[LWR_K][LWR_N/16];
	  __m256i vprime_avx[LWR_N/16];
	  __m256i a_avx[LWR_K][LWR_K][LWR_N/16];
	  __m256i acc[2*LWR_N/16];

	  __m256i pkcl_avx[LWR_K][LWR_N/16];
	  
	  
	  
	  __m256i ciphertext_avx[LWR_N/16];
          __m256i message_avx[LWR_N/16];
		
	  mask_ar[0]=~(0UL);mask_ar[1]=~(0UL);mask_ar[2]=~(0UL);mask_ar[3]=~(0UL);
	  mask_load = _mm256_loadu_si256 ((__m256i const *)mask_ar);

	  mod=_mm256_set1_epi16(LWR_Q-1);
	  mod_p=_mm256_set1_epi16(LWR_P-1);

	  

	    floor_round=_mm256_set1_epi16(4);

	
	    // NOTE: may change after parameter change 	
	    zero = _mm256_set1_epi16(0);
	    one = _mm256_set1_epi16(1);
	    //k23 = _mm256_set1_epi16 (1023);
	    five11 = _mm256_set1_epi16 (511);
	    //two_to6 = _mm256_set1_epi16 (64);
	    two_to5 = _mm256_set1_epi16 (32);
	    //two_to9 = _mm256_set1_epi16 (512);
	    two_to8 = _mm256_set1_epi16 (256);
	    two_to2 = _mm256_set1_epi16 (4);
	    mask = _mm256_set1_epi16 (65535);

 
	//--------------AVX declaration ends------------------
	load_values();
      
	for(i=0;i<LWR_SEEDBYTES;i++){ // Load the seedbytes in the client seed from PK.
		seed[i]=pk[ LWR_POLYVECCOMPRESSEDBYTES + i]; 
	}


	sample_matrix(a, seed);				


  	//poly_getnoise4x(skpv1[0], skpv1[1], skpv1[2], noiseseed, 0x0011, 0x0012, 0x0013, 0x0014);
        GenSecret(skpv1,noiseseed);

	for(i=0;i<LWR_K;i++){ //this code is same as sever code but seed is freshly sampled so different values.
		//poly_getnoise(skpv1[i], noiseseed, nonce++); //generate secret from constant-time binomial distribution
		for(j=0; j<LWR_N/16; j++){
		    sk_avx[i][j] = _mm256_loadu_si256 ((__m256i const *) (&skpv1[i][j*16]));
		}
  	}

	  for(i=0;i<LWR_K;i++){ //load the vector a in avx
		  for(j=0;j<LWR_K;j++){
			  for(k=0;k<LWR_N/16;k++){
				a_avx[i][j][k]=_mm256_loadu_si256 ((__m256i const *) (&a[i].vec[j].coeffs[k*16]));
			  }
		  }
 	 }


	//-----------------matrix-vector multiplication and rounding

	for(i=0;i<LWR_K;i++){
		for(j=0;j<LWR_N/16;j++){
			res_avx[i][j]=_mm256_xor_si256(res_avx[i][j],res_avx[i][j]);
		}
	}	

	for(i=0;i<LWR_K;i++){
		for(j=0;j<LWR_K;j++){
			toom_cook_4way_avx(a_avx[j][i], sk_avx[j], LWR_Q, acc);

			for(k=0;k<LWR_N/16;k++){
				res_avx[i][k]=_mm256_add_epi16(res_avx[i][k],acc[k]);
				res_avx[i][k]=_mm256_and_si256(res_avx[i][k],mod); //reduction mod p
				acc[k]=_mm256_xor_si256(acc[k],acc[k]); //clear the accumulator
			}
			
		}
	}

	
	  //-----now rounding

	for(i=0;i<LWR_K;i++){ //shift right 3 bits
		for(j=0;j<LWR_N/16;j++){
			res_avx[i][j]=_mm256_add_epi16 (res_avx[i][j], floor_round);
			res_avx[i][j]=_mm256_srli_epi16 (res_avx[i][j], 3);
			res_avx[i][j]=_mm256_and_si256 (res_avx[i][j], mod);			

		}
	}



	//-----this result should be put in b_prime for later use in server.
	for(i=0;i<LWR_K;i++){ // first store in 16 bit arrays
		  for(j=0;j<LWR_N/16;j++){
			_mm256_maskstore_epi32 ((int *)(temp[i]+j*16), mask_load, res_avx[i][j]);
		  }
	  }
	
	LWR_pack10bit(ciphertext,temp);


//*******************************************************************client matrix-vector multiplication ends************************************

	//------now calculate the v'

	//-------unpack the public_key
	LWR_un_pack10bit(pk, pkcl); //pkcl is the b in the potocol

	for(i=0;i<LWR_K;i++){
		for(j=0; j<LWR_N/16; j++){
		    pkcl_avx[i][j] = _mm256_loadu_si256 ((__m256i const *) (&pkcl[i][j*16]));
		}
	}


	for(k=0;k<LWR_N/16;k++){
		vprime_avx[k]=_mm256_xor_si256(vprime_avx[k],vprime_avx[k]);
	}

	// vector-vector scalar multiplication with mod p
	clock1=cpucycles();
	count_mul++;
	for(j=0;j<LWR_K;j++){
		toom_cook_4way_avx(pkcl_avx[j], sk_avx[j], LWR_P, acc);

			for(k=0;k<LWR_N/16;k++){
				vprime_avx[k]=_mm256_add_epi16(vprime_avx[k],acc[k]);
				vprime_avx[k]=_mm256_and_si256(vprime_avx[k],mod_p); //reduction
				acc[k]=_mm256_xor_si256(acc[k],acc[k]); //clear the accumulator
			}
	}
	clock2=cpucycles();
	clock_mul=clock_mul+(clock2-clock1);


	// unpack message_received;
	for(j=0; j<LWR_KEYBYTES; j++)
	{
		for(i=0; i<8; i++)
		{
			message[8*j+i] = ((message_received[j]>>i) & 0x01);
		}
	}
	// message encoding
	for(i=0; i<LWR_N/16; i++)
	{
		message_avx[i] = _mm256_loadu_si256 ((__m256i const *) (&message[i*16]));
		message_avx[i] = _mm256_slli_epi16 (message_avx[i], 9);
	}	



	for(k=0;k<LWR_N/16;k++)
	{
		vprime_avx[k]=_mm256_add_epi16(vprime_avx[k], message_avx[k]);
	}

	for(i=0;i<LWR_N/16;i++)
	ciphertext_gen(vprime_avx[i], ciphertext_avx+i);

	for(j=0;j<LWR_N/16;j++)//extract the recon
	{
			_mm256_maskstore_epi32 ((int *) (temp[0]+j*16), mask_load, ciphertext_avx[j]);
	}
	
	LWR_pack_4bit(rec_c, temp[0]);

	for(j=0;j<LWR_RECONBYTES_KEM;j++){
		ciphertext[LWR_POLYVECCOMPRESSEDBYTES + j] = rec_c[j];
	}

	clock_sujoy2 = cpucycles();
	clock_load = clock_load + clock_sujoy2 - clock_sujoy1;
}


void indcpa_kem_dec(const unsigned char *sk, const unsigned char *ciphertext, unsigned char message_dec[])
{

	uint32_t i,j,k;
	
	
	uint16_t sksv[LWR_K][LWR_N]; //secret key of the server
	

	uint16_t pksv[LWR_K][LWR_N];
	
	uint16_t recon_ar[LWR_N];
	uint16_t message_dec_unpacked[LWR_KEYBYTES*8];	// one element containes on decrypted bit;

	//--------------AVX declaration------------------
	

	  __m256i mod_p;
	  
	  __m256i v_avx[LWR_N/16];
	  
	  __m256i acc[2*LWR_N/16];

	  __m256i sksv_avx[LWR_K][LWR_N/16];
	  __m256i pksv_avx[LWR_K][LWR_N/16];
	  
	  __m256i recon_avx[LWR_N/16];
	  __m256i message_dec_unpacked_avx[LWR_N/16];

	  mask_ar[0]=~(0UL);mask_ar[1]=~(0UL);mask_ar[2]=~(0UL);mask_ar[3]=~(0UL);
	  mask_load = _mm256_loadu_si256 ((__m256i const *)mask_ar);

	  mod_p=_mm256_set1_epi16(LWR_P-1);



	    // NOTE: may change after parameter change 	
	    zero = _mm256_set1_epi16(0);
	    one = _mm256_set1_epi16(1);
	    //k23 = _mm256_set1_epi16 (1023);
	    five11 = _mm256_set1_epi16 (511);
	    //two_to6 = _mm256_set1_epi16 (64);
	    two_to5 = _mm256_set1_epi16 (32);
	    //two_to9 = _mm256_set1_epi16 (512);
	    two_to8 = _mm256_set1_epi16 (256);
	    two_to2 = _mm256_set1_epi16 (4);
	    mask = _mm256_set1_epi16 (65535);
	//--------------AVX declaration ends------------------
	
 	load_values();

	//-------unpack the public_key

	LWR_un_pack13bit(sk, sksv); //sksv is the secret-key
	LWR_un_pack10bit(ciphertext, pksv); //pksv is the ciphertext

	LWR_un_pack4bit(ciphertext + LWR_POLYVECCOMPRESSEDBYTES, recon_ar); //ciphertext_ar is the reconciliation



	for(i=0;i<LWR_K;i++){
		for(j=0; j<LWR_N/16; j++){
		    sksv_avx[i][j] = _mm256_loadu_si256 ((__m256i const *) (&sksv[i][j*16]));
		    pksv_avx[i][j] = _mm256_loadu_si256 ((__m256i const *) (&pksv[i][j*16]));
		}
	}

	for(j=0;j<LWR_N/16;j++){
		recon_avx[j] = _mm256_loadu_si256 ((__m256i const *) (&recon_ar[j*16]));
	}

	for(i=0;i<LWR_N/16;i++){
		v_avx[i]=_mm256_xor_si256(v_avx[i],v_avx[i]);
	}


	// vector-vector scalar multiplication with mod p
	for(j=0;j<LWR_K;j++){

		toom_cook_4way_avx(pksv_avx[j], sksv_avx[j], LWR_P, acc);

			for(k=0;k<LWR_N/16;k++){
				v_avx[k]=_mm256_add_epi16(v_avx[k],acc[k]);
				v_avx[k]=_mm256_and_si256(v_avx[k],mod_p); //reduction
				acc[k]=_mm256_xor_si256(acc[k],acc[k]); //clear the accumulator
			}
	}


	for(i=0;i<LWR_N/16;i++){
		    recon(v_avx[i], recon_avx[i], message_dec_unpacked_avx+i);
	}
	

	for(i=0; i<LWR_N/16; i++)
	_mm256_maskstore_epi32 ((int *)(message_dec_unpacked+i*16), mask_load, message_dec_unpacked_avx[i]);
	

	// pack decrypted message
	for(j=0; j<LWR_KEYBYTES; j++)
	{
		message_dec[j] = 0;
		for(i=0; i<8; i++)
		message_dec[j] = message_dec[j] | (message_dec_unpacked[j*8 + i] <<i);
	} 

}

