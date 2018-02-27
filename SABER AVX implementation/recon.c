#include <stdio.h>
#include "recon.h"



__m256i floor_special(__m256i a_avx)
{
    __m256i positive, negative, abs_negative, temp, floor_adjustment, result;

    positive = _mm256_max_epi16 (a_avx, zero);    // keeps the +ve numbers
    negative = _mm256_min_epi16 (a_avx, zero);       // keeps the -ve numbers
    positive = _mm256_srli_epi16 (positive, 9);    // floor(positive/2^10) done.


    abs_negative = _mm256_sub_epi16 (zero, negative);
    temp = _mm256_srli_epi16 (abs_negative, 9);    // >>9
    abs_negative = _mm256_and_si256 (abs_negative, five11);        // keeps the least 9 bits of each
    floor_adjustment = _mm256_cmpgt_epi16 (abs_negative, zero);
    negative = _mm256_add_epi16(temp, floor_adjustment);

    result = _mm256_add_epi16(positive, negative);
    result = _mm256_and_si256(result, one);
    return(result);
}



// generates K1 and recon_data from b_avx
void recon_gen(__m256i b_avx, __m256i *K1, __m256i *recon_data)
{
    __m256i temp;
    b_avx=_mm256_add_epi16(b_avx,two_to2);
    *K1 = _mm256_srli_epi16 (b_avx, 9);    // K1 = b>>9; hence 1 bit per coefficient
    temp = _mm256_and_si256 (b_avx, five11);    // keeps the least 9 bits of temp
    *recon_data = _mm256_srli_epi16 (temp, 6);    // recon_data = [8:6] bits

}



// generates binary K2 from a_avx and recon_data
void recon(__m256i a_avx, __m256i recon_data, __m256i *K2)
{
    __m256i temp;

    //temp = a - (recon_data<<6) - 2^5 + 2^8;
    temp = _mm256_slli_epi16 (recon_data, 6);    // temp = recon_data<<6
    temp = _mm256_sub_epi16(a_avx, temp);        // temp = a - (recon_data<<7)
    temp = _mm256_sub_epi16(temp, two_to5);        // temp = a - (recon_data<<7) - 2^5
    temp = _mm256_add_epi16(temp, two_to8);        // temp = a - (recon_data<<7) - 2^6 + 2^8
    temp= _mm256_add_epi16(temp, two_to2);
    //_mm256_maskstore_epi64 (test_ar, mask, temp);

    //K2 = floor(temp/2^10);
    *K2 = floor_special(temp);

    //int i;
    //int16_t test_ar[16];
    //for(i=0; i<16; i++)
    //printf("test_ar[%d] = %d\n", i, test_ar[i]);

}


// generates ciphertext from input
void ciphertext_gen(__m256i b_avx, __m256i *ciphertext)
{
 
    *ciphertext = _mm256_srli_epi16 (b_avx, 6);    // ciphertext = b>>6; hence 4 bits per coefficient
 
}
