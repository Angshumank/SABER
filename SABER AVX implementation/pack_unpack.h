#ifndef PACK_UNPACK_H
#define PACK_UNPACK_H

#include <stdio.h>
#include<stdint.h>
#include "LWR_params.h"

void LWR_pack11bit(uint8_t *pk,  uint16_t skpv[LWR_K][LWR_N]);

void LWR_un_pack11bit(uint8_t *pk, uint16_t data[LWR_K][LWR_N]);

void LWR_pack14bit(uint8_t *sk,  uint16_t skpv[LWR_K][LWR_N]);

void LWR_un_pack14bit(uint8_t *sk,  uint16_t skpv[LWR_K][LWR_N]);

void LWR_pack_3bit(uint8_t *bytes, uint16_t *data);

void LWR_un_pack3bit(uint8_t *bytes, uint16_t *data);

void LWR_pack10bit(uint8_t *pk,  uint16_t skpv[LWR_K][LWR_N]);

void LWR_un_pack10bit(const unsigned char *pk, uint16_t data[LWR_K][LWR_N]);

void LWR_pack13bit(uint8_t *sk,  uint16_t skpv[LWR_K][LWR_N]);

void LWR_poly_un_pack13bit(const unsigned char *bytes, uint16_t data[LWR_N]);

void LWR_un_pack13bit(const unsigned char *sk,  uint16_t skpv[LWR_K][LWR_N]);

void LWR_pack_4bit(uint8_t *bytes, uint16_t *data);

void LWR_un_pack4bit(const unsigned char *bytes, uint16_t *recon_ar);


#endif
