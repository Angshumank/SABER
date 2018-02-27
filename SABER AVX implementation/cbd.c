/*---------------------------------------------------------------------
This file has been adapted from the implementation 
(available at, Public Domain https://github.com/pq-crystals/kyber) 
of "CRYSTALS – Kyber: a CCA-secure module-lattice-based KEM"
by : Joppe Bos, Leo Ducas, Eike Kiltz, Tancrede Lepoint, 
Vadim Lyubashevsky, John M. Schanck, Peter Schwabe & Damien stehle
----------------------------------------------------------------------*/

#include "cbd.h"

static uint32_t load_littleendian(const unsigned char *x)
{
  return x[0] | (((uint32_t)x[1]) << 8) | (((uint32_t)x[2]) << 16) | (((uint32_t)x[3]) << 24);
}


void cbd(uint16_t *r, const unsigned char *buf)
{
#if LWR_K != 3
#error "poly_getnoise in poly.c only supports k=3"
#endif

  uint32_t t,d, a[4], b[4];
  int i,j;

  uint16_t Qmod_minus1=LWR_Q-1;

  for(i=0;i<LWR_N/4;i++)
  {
    t = load_littleendian(buf+4*i);
    d = 0;
    for(j=0;j<4;j++)
      d += (t >> j) & 0x11111111;

    a[0] =  d & 0xf;
    b[0] = (d >>  4) & 0xf;
    a[1] = (d >>  8) & 0xf;
    b[1] = (d >> 12) & 0xf;
    a[2] = (d >> 16) & 0xf;
    b[2] = (d >> 20) & 0xf;
    a[3] = (d >> 24) & 0xf;
    b[3] = (d >> 28);
    
    r[4*i+0] = (uint16_t)(a[0]  - b[0]) & Qmod_minus1;
    r[4*i+1] = (uint16_t)(a[1]  - b[1]) & Qmod_minus1;
    r[4*i+2] = (uint16_t)(a[2]  - b[2]) & Qmod_minus1;
    r[4*i+3] = (uint16_t)(a[3]  - b[3]) & Qmod_minus1;


  }
}
