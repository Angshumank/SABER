#include <immintrin.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include<math.h>
#include "LWR_params.h"

__m256i floor_special(__m256i a_avx);

void recon_gen(__m256i b_avx, __m256i *K1, __m256i *recon_data);

void recon(__m256i a_avx, __m256i recon_data, __m256i *K2);

void ciphertext_gen(__m256i b_avx, __m256i *ciphertext);

//__m256i zero, one, k23, five11, two_to6, two_to5, two_to9, two_to8, mask;
__m256i zero, one, five11, two_to5, two_to8, two_to2, mask;
