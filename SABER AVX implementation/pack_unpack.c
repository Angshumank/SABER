#include "pack_unpack.h"

void LWR_pack_3bit(uint8_t *bytes, uint16_t *data){

	uint32_t j;
	uint32_t offset_data=0,offset_byte=0;
	
	offset_byte=0;
	for(j=0;j<LWR_N/8;j++){
		offset_byte=3*j;
		offset_data=8*j;
		bytes[offset_byte + 0]= (data[offset_data + 0] & 0x7) | ( (data[offset_data + 1] & 0x7)<<3 ) | ((data[offset_data + 2] & 0x3)<<6);
		bytes[offset_byte + 1]= ((data[offset_data + 2] >> 2 ) & 0x01)  | ( (data[offset_data + 3] & 0x7)<<1 ) | ( (data[offset_data + 4] & 0x7)<<4 ) | (((data[offset_data + 5] >> 2 ) & 0x01)<<7);
		bytes[offset_byte + 2]= ((data[offset_data + 5] >> 1 ) & 0x03) | ( (data[offset_data + 6] & 0x7)<<2 ) | ( (data[offset_data + 7] & 0x7)<<5 );
	}
}

void LWR_un_pack3bit(uint8_t *bytes, uint16_t *data){

	uint32_t j;
	uint32_t offset_data=0,offset_byte=0;	
	
	offset_byte=0;
	for(j=0;j<LWR_N/8;j++){
		offset_byte=3*j;
		offset_data=8*j;
		data[offset_data + 0] = (bytes[offset_byte+0])&0x07;
		data[offset_data + 1] = ( (bytes[offset_byte+0])>>3 )&0x07;
		data[offset_data + 2] = ( ( (bytes[offset_byte+0])>>6 )&0x03) | ( ( (bytes[offset_byte+1])&0x01)<<2 );
		data[offset_data + 3] = ( (bytes[offset_byte+1])>>1 )&0x07;
		data[offset_data + 4] = ( (bytes[offset_byte+1])>>4 )&0x07;
		data[offset_data + 5] = ( ( (bytes[offset_byte+1])>>7 )&0x01) | ( ( (bytes[offset_byte+2])&0x03)<<1 );
		data[offset_data + 6] = ( (bytes[offset_byte+2]>>2)&0x07 );
		data[offset_data + 7] = ( (bytes[offset_byte+2]>>5)&0x07 );
	}

}


void LWR_pack_4bit(uint8_t *bytes, uint16_t *data){

	uint32_t j;
	uint32_t offset_data=0;
	
	for(j=0;j<LWR_N/2;j++)
	{
		offset_data=2*j;
		bytes[j]= (data[offset_data] & 0x0f) | ( (data[offset_data + 1] & 0x0f)<<4 );
	}
}

void LWR_un_pack4bit(const unsigned char *bytes, uint16_t *recon_ar){

	uint32_t j;
	uint32_t offset_data=0;
	
	for(j=0;j<LWR_N/2;j++)
	{
		offset_data=2*j;
		recon_ar[offset_data] = bytes[j] & 0x0f;
		recon_ar[offset_data+1] = (bytes[j]>>4) & 0x0f;

		//masked_message_ar[offset_data] = (bytes[j]>>3) & 0x01;
		//masked_message_ar[offset_data+1] = (bytes[j]>>7) & 0x01;

	}
}

void LWR_pack10bit(uint8_t *bytes, uint16_t data[LWR_K][LWR_N]){
	
	uint32_t i,j;
	uint32_t offset_data=0,offset_byte=0,offset_byte1=0;	
	
	offset_byte=0;
	for(i=0;i<LWR_K;i++){
		offset_byte1=i*(LWR_N*10)/8;
		for(j=0;j<LWR_N/4;j++){
			offset_byte=offset_byte1+5*j;
			offset_data=4*j;
			bytes[offset_byte + 0]= ( data[i][ offset_data + 0 ] & (0xff));

			bytes[offset_byte + 1]= ( (data[i][ offset_data + 0 ] >>8) & 0x03 ) | ((data[i][ offset_data + 1 ] & 0x3f) << 2);

			bytes[offset_byte + 2]= ( (data[i][ offset_data + 1 ] >>6) & 0x0f ) | ( (data[i][ offset_data + 2 ] &0x0f) << 4);

			bytes[offset_byte + 3]= ( (data[i][ offset_data + 2 ] >>4) & 0x3f ) | ((data[i][ offset_data + 3 ] & 0x03) << 6);

			bytes[offset_byte + 4]= ( (data[i][ offset_data + 3 ] >>2) & 0xff );
		}
	}


}

void LWR_un_pack10bit(const unsigned char *bytes, uint16_t data[LWR_K][LWR_N]){
	
	uint32_t i,j;
	uint32_t offset_data=0,offset_byte=0,offset_byte1=0;	
	
	offset_byte=0;
	for(i=0;i<LWR_K;i++){
		offset_byte1=i*(LWR_N*10)/8;
		for(j=0;j<LWR_N/4;j++){
			offset_byte=offset_byte1+5*j;
			offset_data=4*j;
			data[i][offset_data + 0]= ( bytes[ offset_byte + 0 ] & (0xff)) |  ((bytes[ offset_byte + 1 ] & 0x03)<<8);
			data[i][offset_data + 1]= ( (bytes[ offset_byte + 1 ]>>2) & (0x3f)) |  ((bytes[ offset_byte + 2 ] & 0x0f)<<6);		
			data[i][offset_data + 2]= ( (bytes[ offset_byte + 2 ]>>4) & (0x0f)) |  ((bytes[ offset_byte + 3 ] & 0x3f)<<4);
			data[i][offset_data + 3]= ( (bytes[ offset_byte + 3 ]>>6) & (0x03)) |  ((bytes[ offset_byte + 4 ] & 0xff)<<2);		

		}
	}


}


void LWR_pack13bit(uint8_t *bytes, uint16_t data[LWR_K][LWR_N]){
	
	uint32_t i,j;
	uint32_t offset_data=0,offset_byte=0,offset_byte1=0;	
	
	offset_byte=0;
	for(i=0;i<LWR_K;i++){
		offset_byte1=i*(LWR_N*13)/8;
		for(j=0;j<LWR_N/8;j++){
			offset_byte=offset_byte1+13*j;
			offset_data=8*j;
			bytes[offset_byte + 0]= ( data[i][ offset_data + 0 ] & (0xff));

			bytes[offset_byte + 1]= ( (data[i][ offset_data + 0 ] >>8) & 0x1f ) | ((data[i][ offset_data + 1 ] & 0x07) << 5);

			bytes[offset_byte + 2]= ( (data[i][ offset_data + 1 ] >>3) & 0xff );

			bytes[offset_byte + 3]= ( (data[i][ offset_data + 1 ] >>11) & 0x03 ) | ((data[i][ offset_data + 2 ] & 0x3f) << 2);

			bytes[offset_byte + 4]= ( (data[i][ offset_data + 2 ] >>6) & 0x7f ) | ( (data[i][ offset_data + 3 ] & 0x01) << 7 );

			bytes[offset_byte + 5]= ( (data[i][ offset_data + 3 ] >>1) & 0xff );

			bytes[offset_byte + 6]= ( (data[i][ offset_data + 3 ] >>9) & 0x0f ) | ( (data[i][ offset_data + 4 ] & 0x0f) << 4 );

			bytes[offset_byte + 7]= ( (data[i][ offset_data + 4] >>4) & 0xff );

			bytes[offset_byte + 8]= ( (data[i][ offset_data + 4 ] >>12) & 0x01 ) | ( (data[i][ offset_data + 5 ] & 0x7f) << 1 );

			bytes[offset_byte + 9]= ( (data[i][ offset_data + 5 ] >>7) & 0x3f ) | ( (data[i][ offset_data + 6 ] & 0x03) << 6 );

			bytes[offset_byte + 10]= ( (data[i][ offset_data + 6 ] >>2) & 0xff );

			bytes[offset_byte + 11]= ( (data[i][ offset_data + 6 ] >>10) & 0x07 ) | ( (data[i][ offset_data + 7 ] & 0x1f) << 3 );

			bytes[offset_byte + 12]= ( (data[i][ offset_data + 7 ] >>5) & 0xff );

		}
	}


}

void LWR_un_pack13bit(const unsigned char *bytes, uint16_t data[LWR_K][LWR_N]){
	
	uint32_t i,j;
	uint32_t offset_data=0,offset_byte=0,offset_byte1=0;	
	
	offset_byte=0;
	for(i=0;i<LWR_K;i++){
		offset_byte1=i*(LWR_N*13)/8;
		for(j=0;j<LWR_N/8;j++){
			offset_byte=offset_byte1+13*j;
			offset_data=8*j;
			data[i][offset_data + 0]= ( bytes[ offset_byte + 0 ] & (0xff)) | ((bytes[offset_byte + 1] & 0x1f)<<8);
			data[i][offset_data + 1]= ( bytes[ offset_byte + 1 ]>>5 & (0x07)) | ((bytes[offset_byte + 2] & 0xff)<<3) | ((bytes[offset_byte + 3] & 0x03)<<11);
			data[i][offset_data + 2]= ( bytes[ offset_byte + 3 ]>>2 & (0x3f)) | ((bytes[offset_byte + 4] & 0x7f)<<6);
			data[i][offset_data + 3]= ( bytes[ offset_byte + 4 ]>>7 & (0x01)) | ((bytes[offset_byte + 5] & 0xff)<<1) | ((bytes[offset_byte + 6] & 0x0f)<<9);
			data[i][offset_data + 4]= ( bytes[ offset_byte + 6 ]>>4 & (0x0f)) | ((bytes[offset_byte + 7] & 0xff)<<4) | ((bytes[offset_byte + 8] & 0x01)<<12);
			data[i][offset_data + 5]= ( bytes[ offset_byte + 8]>>1 & (0x7f)) | ((bytes[offset_byte + 9] & 0x3f)<<7);
			data[i][offset_data + 6]= ( bytes[ offset_byte + 9]>>6 & (0x03)) | ((bytes[offset_byte + 10] & 0xff)<<2) | ((bytes[offset_byte + 11] & 0x07)<<10);
			data[i][offset_data + 7]= ( bytes[ offset_byte + 11]>>3 & (0x1f)) | ((bytes[offset_byte + 12] & 0xff)<<5);
		}
	}


}

void LWR_poly_un_pack13bit(const unsigned char *bytes, uint16_t data[LWR_N]){
	
	uint32_t j;
	uint32_t offset_data=0,offset_byte=0;	
	
	offset_byte=0;
	//for(i=0;i<LWR_K;i++){
		//i=0;
		//offset_byte1=i*(LWR_N*13)/8;
		for(j=0;j<LWR_N/8;j++){
			//offset_byte=offset_byte1+13*j;
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
	//}


}


void LWR_pack11bit(uint8_t *bytes,  uint16_t data[LWR_K][LWR_N]){
/*This function packs 11 bit data stream into 8 bits of data.
*/
	uint32_t i,j;
	uint32_t offset_data=0,offset_byte=0,offset_byte1=0;	
	
	offset_byte=0;
	for(i=0;i<LWR_K;i++){
		offset_byte1=i*(LWR_N*11)/8;
		for(j=0;j<LWR_N/8;j++){
			offset_byte=offset_byte1+11*j;
			offset_data=8*j;
			bytes[offset_byte + 0]= ( data[i][ offset_data + 0 ] & (0xff));

			bytes[offset_byte + 1]= ( (data[i][ offset_data + 0 ] >>8) & 0x07 ) | ((data[i][ offset_data + 1 ] & 0x1f) << 3);

			bytes[offset_byte + 2]= ( (data[i][ offset_data + 1 ] >>5) & 0x3f ) | ((data[i][ offset_data + 2 ] & 0x03) << 6);

			bytes[offset_byte + 3]= ( (data[i][ offset_data + 2 ] >>2) & 0xff );

			bytes[offset_byte + 4]= ( (data[i][ offset_data + 2 ] >>10) & 0x01 ) | ((data[i][ offset_data + 3 ] & 0x7f) << 1);

			bytes[offset_byte + 5]= ( (data[i][ offset_data + 3 ] >>7) & 0x0f ) | ((data[i][ offset_data + 4 ] & 0x0f) << 4);

			bytes[offset_byte + 6]= ( (data[i][ offset_data + 4 ] >>4) & 0x7f ) | ((data[i][ offset_data + 5 ] & 0x01) << 7);

			bytes[offset_byte + 7]= ( (data[i][ offset_data + 5 ] >>1) & 0xff );

			bytes[offset_byte + 8]= ( (data[i][ offset_data + 5 ] >>9) & 0x03 ) | ((data[i][ offset_data + 6 ] & 0x3f) << 2);

			bytes[offset_byte + 9]= ( (data[i][ offset_data + 6 ] >>6) & 0x1f ) | ((data[i][ offset_data + 7 ] & 0x07) << 5);

			bytes[offset_byte + 10]=( (data[i][ offset_data + 7 ] >>3) & 0xff );
			
		}
	}

}

void LWR_un_pack11bit(uint8_t *bytes, uint16_t data[LWR_K][LWR_N]){
	
	uint32_t i,j;
	uint32_t offset_data=0,offset_byte=0,offset_byte1=0;


	for(i=0;i<LWR_K;i++){
		offset_byte1=i*(LWR_N*11)/8;
		for(j=0;j<LWR_N/8;j++){
			offset_byte=offset_byte1+11*j;
			offset_data=8*j;

			data[i][offset_data+ 0]=(bytes[offset_byte+0]) | ( (bytes[offset_byte + 1] & 0x07) << 8 );

			data[i][offset_data+ 1]= ( (bytes[offset_byte+1] >>3)&0x1f) | ( (bytes[offset_byte + 2] & 0x3f) << 5 );

			data[i][offset_data+ 2]= ( (bytes[offset_byte+2] >>6)&0x03) | ( (bytes[offset_byte + 3] & 0xff) << 2 ) | ( (bytes[offset_byte + 4] & 0x01) << 10 );

			data[i][offset_data+ 3]= ( (bytes[offset_byte+4] >>1)&0x7f) | ( (bytes[offset_byte + 5] & 0x0f) << 7 );

			data[i][offset_data+ 4]= ( (bytes[offset_byte+5] >>4)&0x0f) | ( (bytes[offset_byte + 6] & 0x7f) << 4 );

			data[i][offset_data+ 5]= ( (bytes[offset_byte+6] >>7)&0x01) | ( (bytes[offset_byte + 7] & 0xff) << 1 ) | ( (bytes[offset_byte + 8] & 0x03) << 9 );

			data[i][offset_data+ 6]= ( (bytes[offset_byte+8] >>2)&0x3f) | ( (bytes[offset_byte + 9] & 0x1f) << 6 );

			data[i][offset_data+ 7]= ( (bytes[offset_byte+9] >>5)&0x07) | ( (bytes[offset_byte + 10] & 0xff) << 3 );
		}
	}


}

void LWR_pack14bit(uint8_t *bytes, uint16_t data[LWR_K][LWR_N]){
	
	uint32_t i,j;
	uint32_t offset_data=0,offset_byte=0,offset_byte1=0;	
	
	offset_byte=0;
	for(i=0;i<LWR_K;i++){
		offset_byte1=i*(LWR_N*14)/8;
		for(j=0;j<LWR_N/4;j++){
			offset_byte=offset_byte1+7*j;
			offset_data=4*j;
			bytes[offset_byte + 0]= ( data[i][ offset_data + 0 ] & (0xff));

			bytes[offset_byte + 1]= ( (data[i][ offset_data + 0 ] >>8) & 0x3f ) | ((data[i][ offset_data + 1 ] & 0x03) << 6);

			bytes[offset_byte + 2]= ( (data[i][ offset_data + 1 ] >>2) & 0xff );

			bytes[offset_byte + 3]= ( (data[i][ offset_data + 1 ] >>10) & 0x0f ) | ((data[i][ offset_data + 2 ] & 0x0f) << 4);

			bytes[offset_byte + 4]= ( (data[i][ offset_data + 2 ] >>4) & 0xff );

			bytes[offset_byte + 5]= ( (data[i][ offset_data + 2 ] >>12) & 0x03 ) | ((data[i][ offset_data + 3 ] & 0x3f) << 2);

			bytes[offset_byte + 6]= ( (data[i][ offset_data + 3 ] >>6) & 0xff );
		}
	}


}


void LWR_un_pack14bit(uint8_t *bytes, uint16_t data[LWR_K][LWR_N]){
	
	uint32_t i,j;
	uint32_t offset_data=0,offset_byte=0,offset_byte1=0;	
	
	offset_byte=0;
	for(i=0;i<LWR_K;i++){
		offset_byte1=i*(LWR_N*14)/8;
		for(j=0;j<LWR_N/4;j++){
			offset_byte=offset_byte1+7*j;
			offset_data=4*j;
			data[i][offset_data+ 0]=(bytes[offset_byte+0]&0xff) | ( (bytes[offset_byte + 1] & 0x3f) << 8 );

			data[i][offset_data+ 1]= ( (bytes[offset_byte+1] >>6)&0x03) | ((bytes[offset_byte+2]&0xff)<<2 ) | ( (bytes[offset_byte + 3] & 0x0f) << 10 );

			data[i][offset_data+ 2]= ( (bytes[offset_byte+3] >>4)&0x0f) | ( (bytes[offset_byte + 4] ) << 4 ) | ( (bytes[offset_byte + 5] & 0x03) << 12 );

			data[i][offset_data+ 3]= ( (bytes[offset_byte+5] >>2)&0x3f) | ( (bytes[offset_byte + 6] ) << 6 );
		}
	}


}

