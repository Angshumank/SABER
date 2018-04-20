/*---------------------------------------------------------------------
This file has been adapted from the implementation 
(available at, Public Domain https://github.com/pq-crystals/kyber) 
of "CRYSTALS – Kyber: a CCA-secure module-lattice-based KEM"
by : Joppe Bos, Leo Ducas, Eike Kiltz, Tancrede Lepoint, 
Vadim Lyubashevsky, John M. Schanck, Peter Schwabe & Damien stehle
----------------------------------------------------------------------*/

#include "api.h"
#include<stdint.h>
#include<stdio.h>//added
#include "SABER_params.h"
#include <immintrin.h>

static uint64_t load_littleendian(const unsigned char *x, int bytes)
{
  int i;
  uint64_t r = x[0];
  for(i=1;i<bytes;i++)
    r |= (uint64_t)x[i] << (8*i);
  return r;
}


void cbd(uint16_t *r, const unsigned char *buf)
{
	uint16_t Qmod_minus1=SABER_Q-1;

#if Saber_type == 3
  uint32_t t,d, a[4], b[4];
  int i,j;

  for(i=0;i<SABER_N/4;i++)
  {
    t = load_littleendian(buf+3*i,3);
    d = 0;
    for(j=0;j<3;j++)
      d += (t >> j) & 0x249249;

    a[0] =  d & 0x7;
    b[0] = (d >>  3) & 0x7;
    a[1] = (d >>  6) & 0x7;
    b[1] = (d >>  9) & 0x7;
    a[2] = (d >> 12) & 0x7;
    b[2] = (d >> 15) & 0x7;
    a[3] = (d >> 18) & 0x7;
    b[3] = (d >> 21);

    r[4*i+0] = (uint16_t)(a[0]  - b[0]) & Qmod_minus1;
    r[4*i+1] = (uint16_t)(a[1]  - b[1]) & Qmod_minus1;
    r[4*i+2] = (uint16_t)(a[2]  - b[2]) & Qmod_minus1;
    r[4*i+3] = (uint16_t)(a[3]  - b[3]) & Qmod_minus1;

  }
#elif Saber_type == 2
  uint32_t t,d, a[4], b[4];
  int i,j;

printf("___________________________________start of print_______________________________\n");

  for(i=0;i<SABER_N/4;i++)
  {

    t = load_littleendian(buf+4*i,4);
    //printf("t : i : %u\t%u\t",i,t);
    d = 0;
    for(j=0;j<4;j++)
      d += (t >> j) & 0x11111111;

	//-----------
    //printf("t : i : %u\t%u\t",i,d);
	//-----------

    a[0] =  d & 0xf;
    b[0] = (d >>  4) & 0xf;
    a[1] = (d >>  8) & 0xf;
    b[1] = (d >> 12) & 0xf;
    a[2] = (d >> 16) & 0xf;
    b[2] = (d >> 20) & 0xf;
    a[3] = (d >> 24) & 0xf;
    b[3] = (d >> 28);
/*
	//-----------
	printf("a0 i : %u\t%u\t",i,a[0]);
	//-----------

	//-----------
	printf("b0 i : %u\t%u\t",i,b[0]);
	//-----------
*/

    r[4*i+0] = (uint16_t)(a[0]  - b[0]) & Qmod_minus1;
    r[4*i+1] = (uint16_t)(a[1]  - b[1]) & Qmod_minus1;
    r[4*i+2] = (uint16_t)(a[2]  - b[2]) & Qmod_minus1;
    r[4*i+3] = (uint16_t)(a[3]  - b[3]) & Qmod_minus1;

	//-----------
	//printf("r0 i : %u,%u\t",4*i+0,(a[0]-b[0])& Qmod_minus1);
	//-----------
	
    
  }
//-------------

	for(i=0;i<SABER_N;i++){
		if(i%8==0)
			printf("\n");
		printf("%u\t",r[i]);
	}

//-------------
printf("\n\n");
//--------------------------
  uint32_t t1[8];//there will be 8 32 bit integer
  uint32_t d1[8];//there will be 8 32 bit integer
  uint32_t const_cbd[8];
  uint32_t mask_const[8];

  uint16_t r1[SABER_N];//delete this
	for(i=0;i<SABER_N;i++)
		r1[i]=0;


  int k;	

  __m256i const_avx, d_avx, t_avx, temp_avx, mask_const_avx, a_avx[4], b_avx[4];

  __m256i mask;
  unsigned long long int mask_ar[4];
  mask_ar[0]=~(0UL);mask_ar[1]=~(0UL);mask_ar[2]=~(0UL);mask_ar[3]=~(0UL);
  mask = _mm256_loadu_si256 ((__m256i const *)mask_ar);


  for(i=0;i<8;i++){

	const_cbd[i]=0x11111111;
	mask_const[i]=0xf;

  }

  const_avx = _mm256_loadu_si256 ((__m256i const *) (&const_cbd));
  mask_const_avx=_mm256_loadu_si256 ((__m256i const *) (&mask_const));

  for(i=0;i<SABER_N/(4*8);i++)
  {
   
	for(j=0;j<8;j++){	
	    t1[j] = load_littleendian(buf+32*i+4*j,4);
	}

	d_avx=_mm256_xor_si256 (d_avx, d_avx);//make d=0

	
	/*for(j=0;j<8;j++){	
		printf("t : i : %u\t%u\t",8*i+j,t1[j]);
	}*/
	

    t_avx=_mm256_loadu_si256 ((__m256i const *) (&t1));//load t	

	for(j=0;j<4;j++){
		temp_avx= _mm256_srli_epi32 (t_avx, j);
		temp_avx=_mm256_and_si256 (temp_avx, const_avx);
		d_avx=_mm256_add_epi32 (d_avx, temp_avx);
		
	} 

	//--------------
	/*_mm256_maskstore_epi32 ((int *)d1, mask, d_avx);
	for(j=0;j<8;j++){	
		printf("t : i : %u\t%u\t",8*i+j,d1[j]);
	}*/
	//--------------

//--------------------------
	a_avx[0]= _mm256_and_si256(d_avx, mask_const_avx);

/*	//--------------
	_mm256_maskstore_epi32 ((int *)d1, mask, a_avx[0]);
	for(j=0;j<8;j++){	
		printf("a0 i : %u\t%u\t",8*i+j,d1[j]);
	}
	printf("\n\n");
	//--------------
*/

	temp_avx= _mm256_srli_epi32(d_avx,4);
	b_avx[0]=_mm256_and_si256 (temp_avx, mask_const_avx);


/*	//--------------
	_mm256_maskstore_epi32 ((int *)d1, mask, b_avx[0]);
	for(j=0;j<8;j++){	
		printf("b0 i : %u\t%u\t",8*i+j,d1[j]);
	}
	printf("\n\n");
	//--------------
*/

	temp_avx= _mm256_srli_epi32(d_avx,8);
	a_avx[1]=_mm256_and_si256 (temp_avx, mask_const_avx);
	temp_avx= _mm256_srli_epi32(d_avx,12);
	b_avx[1]=_mm256_and_si256 (temp_avx, mask_const_avx);

	temp_avx= _mm256_srli_epi32(d_avx,16);
	a_avx[2]=_mm256_and_si256 (temp_avx, mask_const_avx);
	temp_avx= _mm256_srli_epi32(d_avx,20);
	b_avx[2]=_mm256_and_si256 (temp_avx, mask_const_avx);

	temp_avx= _mm256_srli_epi32(d_avx,24);
	a_avx[3]=_mm256_and_si256 (temp_avx, mask_const_avx);
	b_avx[3]= _mm256_srli_epi32(d_avx,28);
//--------------------------------
	
	a_avx[0]=_mm256_sub_epi32 (a_avx[0], b_avx[0]);
	a_avx[1]=_mm256_sub_epi32 (a_avx[1], b_avx[1]);
	a_avx[2]=_mm256_sub_epi32 (a_avx[2], b_avx[2]);
	a_avx[3]=_mm256_sub_epi32 (a_avx[3], b_avx[3]);

	//--------------place the values-------
	_mm256_maskstore_epi32 ((int *)d1, mask, a_avx[0]);
	
	for(j=0;j<8;j++){	
		//printf("r0 i : %u,%u\t",4*(8*i+j)+0,d1[j]&  Qmod_minus1);
		r1[4*(8*i+j)+0]=d1[j] &  Qmod_minus1;
	}
	_mm256_maskstore_epi32 ((int *)d1, mask, a_avx[1]);
	
	for(j=0;j<8;j++){	
		//printf("r0 i : %u,%u\t",4*(8*i+j)+1,d1[j]&  Qmod_minus1);
		r1[4*(8*i+j)+1]=d1[j] &  Qmod_minus1;
	}
	_mm256_maskstore_epi32 ((int *)d1, mask, a_avx[2]);
	
	for(j=0;j<8;j++){	
		//printf("r0 i : %u,%u\t",4*(8*i+j)+2,d1[j]&  Qmod_minus1);
		r1[4*(8*i+j)+2]=d1[j] &  Qmod_minus1;
	}
	_mm256_maskstore_epi32 ((int *)d1, mask, a_avx[3]);
	
	for(j=0;j<8;j++){	
		//printf("r0 i : %u,%u\t",4*(8*i+j)+3,d1[j]&  Qmod_minus1);
		r1[4*(8*i+j)+3]=d1[j] &  Qmod_minus1;
	}
	//printf("\n\n");
	//--------------place the values ends-------

	//_mm256_maskstore_epi32 ((int *)(&r1[2]+32*i), mask, a_avx[0]);

/*
	_mm256_maskstore_epi32 ((int *)(r1+32*i+0), mask, a_avx[0]);
	_mm256_maskstore_epi32 ((int *)(r1+32*i+8), mask, a_avx[1]);
	_mm256_maskstore_epi32 ((int *)(r1+32*i+16), mask, a_avx[2]);
	_mm256_maskstore_epi32 ((int *)(r1+32*i+24), mask, a_avx[3]);
*/
  }


	for(i=0;i<SABER_N;i++){
		if(i%8==0)
			printf("\n");
		printf("%u\t",r1[i] &  Qmod_minus1);
	}

	printf("\n");

printf("___________________________________end of print_______________________________\n");

#elif Saber_type == 1
  uint64_t t,d, a[4], b[4];
  int i,j;

  for(i=0;i<SABER_N/4;i++)
  {
    t = load_littleendian(buf+5*i,5);
    d = 0;
    for(j=0;j<5;j++)
      d += (t >> j) & 0x0842108421UL;

    a[0] =  d & 0x1f;
    b[0] = (d >>  5) & 0x1f;
    a[1] = (d >> 10) & 0x1f;
    b[1] = (d >> 15) & 0x1f;
    a[2] = (d >> 20) & 0x1f;
    b[2] = (d >> 25) & 0x1f;
    a[3] = (d >> 30) & 0x1f;
    b[3] = (d >> 35);

    r[4*i+0] = (uint16_t)(a[0]  - b[0]) & Qmod_minus1;
    r[4*i+1] = (uint16_t)(a[1]  - b[1]) & Qmod_minus1;
    r[4*i+2] = (uint16_t)(a[2]  - b[2]) & Qmod_minus1;
    r[4*i+3] = (uint16_t)(a[3]  - b[3]) & Qmod_minus1;
  }
#else
#error "Unsupported SABER parameter."
#endif
}
