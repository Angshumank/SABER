#define SCM_SIZE 16


unsigned short a0_array[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned short a1_array[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned short a2_array[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned short a3_array[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned short b0_array[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned short b1_array[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned short b2_array[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned short b3_array[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};


unsigned short a_array[SCM_SIZE][16];
unsigned short b_array[SCM_SIZE][16];	

// Schoolbook internal variables
__m256i a0, a1, a2, a3, b0, b1, b2, b3;
__m256i c0, c1, c2, c3, c4, c5, c6;
__m256i temp;
//__m256i mask;

__m256i c_avx[2*SCM_SIZE]; 
__m256i a[SCM_SIZE+2]; 
__m256i b[SCM_SIZE+2]; 
__m256i c_avx_extra[4];

//unsigned long long int mask_ar[4];


void load_avx_input_new(uint16_t* aa, uint16_t* bb, int index)
{
	int i, i_plus1;

	for(i=0; i<SCM_SIZE; i=i+2)
	{
		a_array[i][index] = aa[i];		
		b_array[i][index] = bb[i];		
		
		i_plus1 = i+1;
		a_array[i_plus1][index] = aa[i_plus1];		
		b_array[i_plus1][index] = bb[i_plus1];		
	}
}



void schoolbook_avx_new1()
{
	int i, j;
	int scm_size_1, vv;

	/*
	for(i=0; i<SCM_SIZE; i=i+2)
	{
		a[i] = _mm256_loadu_si256 ((__m256i const *)a_array[i]);
		b[i] = _mm256_loadu_si256 ((__m256i const *)b_array[i]);	

		vv = i+1;
		a[vv] = _mm256_loadu_si256 ((__m256i const *)a_array[vv]);
		b[vv] = _mm256_loadu_si256 ((__m256i const *)b_array[vv]);	

	}
	*/
	/*	
	// the first triangle
	for(i=0; i<SCM_SIZE; i++)
	{
		for(j=0; j<=i; j++)
		{
			if(j==0)
				c_avx[i] = _mm256_mullo_epi16 (a[j], b[i]);	
			
			else
			{
				temp = _mm256_mullo_epi16 (a[j], b[i-j]);
				c_avx[i] = _mm256_add_epi16(c_avx[i], temp);
			}
		}
	}
	*/

	// the first triangle
	for(i=0; i<SCM_SIZE; i++)
	{
		c_avx[i] = _mm256_mullo_epi16 (a[0], b[i]);	
		for(j=1; j<=i; j++)
		{
				temp = _mm256_mullo_epi16 (a[j], b[i-j]);
				c_avx[i] = _mm256_add_epi16(c_avx[i], temp);
		}
	}


	scm_size_1 = SCM_SIZE-1;
	
	/*
	// the second triangle
	for(i=1; i<SCM_SIZE; i++)
	{
		for(j=0; j<SCM_SIZE-i; j++)
		{
			if(j==0)
				c_avx[scm_size_1+i] = _mm256_mullo_epi16 (a[i], b[scm_size_1]);	
			
			else
			{
				temp = _mm256_mullo_epi16 (a[i+j], b[scm_size_1-j]);
				c_avx[scm_size_1+i] = _mm256_add_epi16(c_avx[scm_size_1+i], temp);
			}
		}
	}
	*/

	// the second triangle
	for(i=1; i<SCM_SIZE; i++)
	{
		c_avx[scm_size_1+i] = _mm256_mullo_epi16 (a[i], b[scm_size_1]);	
		vv = scm_size_1+i;
		for(j=1; j<SCM_SIZE-i; j++)
		{
				temp = _mm256_mullo_epi16 (a[i+j], b[scm_size_1-j]);
				c_avx[vv] = _mm256_add_epi16(c_avx[vv], temp);
		}
	}
	c_avx[2*SCM_SIZE-1] = _mm256_set_epi64x(0, 0, 0, 0);

}


void schoolbook_avx_new()
{
	int i, j;
	int scm_size_1, vv;

	
	for(i=0; i<SCM_SIZE; i=i+2)
	{
		a[i] = _mm256_loadu_si256 ((__m256i const *)a_array[i]);
		b[i] = _mm256_loadu_si256 ((__m256i const *)b_array[i]);	

		vv = i+1;
		a[vv] = _mm256_loadu_si256 ((__m256i const *)a_array[vv]);
		b[vv] = _mm256_loadu_si256 ((__m256i const *)b_array[vv]);	

	}
	
	/*	
	// the first triangle
	for(i=0; i<SCM_SIZE; i++)
	{
		for(j=0; j<=i; j++)
		{
			if(j==0)
				c_avx[i] = _mm256_mullo_epi16 (a[j], b[i]);	
			
			else
			{
				temp = _mm256_mullo_epi16 (a[j], b[i-j]);
				c_avx[i] = _mm256_add_epi16(c_avx[i], temp);
			}
		}
	}
	*/

	// the first triangle
	for(i=0; i<SCM_SIZE; i++)
	{
		c_avx[i] = _mm256_mullo_epi16 (a[0], b[i]);	
		for(j=1; j<=i; j++)
		{
				temp = _mm256_mullo_epi16 (a[j], b[i-j]);
				c_avx[i] = _mm256_add_epi16(c_avx[i], temp);
		}
	}


	scm_size_1 = SCM_SIZE-1;
	
	/*
	// the second triangle
	for(i=1; i<SCM_SIZE; i++)
	{
		for(j=0; j<SCM_SIZE-i; j++)
		{
			if(j==0)
				c_avx[scm_size_1+i] = _mm256_mullo_epi16 (a[i], b[scm_size_1]);	
			
			else
			{
				temp = _mm256_mullo_epi16 (a[i+j], b[scm_size_1-j]);
				c_avx[scm_size_1+i] = _mm256_add_epi16(c_avx[scm_size_1+i], temp);
			}
		}
	}
	*/

	// the second triangle
	for(i=1; i<SCM_SIZE; i++)
	{
		c_avx[scm_size_1+i] = _mm256_mullo_epi16 (a[i], b[scm_size_1]);	
		vv = scm_size_1+i;
		for(j=1; j<SCM_SIZE-i; j++)
		{
				temp = _mm256_mullo_epi16 (a[i+j], b[scm_size_1-j]);
				c_avx[vv] = _mm256_add_epi16(c_avx[vv], temp);
		}
	}


}

/*
int schoolbook_avx()
{
	int i;
	unsigned short c_array[16];

	a0 = _mm256_loadu_si256 ((__m256i const *)a0_array);
	a1 = _mm256_loadu_si256 ((__m256i const *)a1_array);
	a2 = _mm256_loadu_si256 ((__m256i const *)a2_array);
	a3 = _mm256_loadu_si256 ((__m256i const *)a3_array);

	b0 = _mm256_loadu_si256 ((__m256i const *)b0_array);
	b1 = _mm256_loadu_si256 ((__m256i const *)b1_array);
	b2 = _mm256_loadu_si256 ((__m256i const *)b2_array);
	b3 = _mm256_loadu_si256 ((__m256i const *)b3_array);

	// c0 = a0*b0
	c0 = _mm256_mullo_epi16 (a0, b0);

	// c1 = a0*b1+a1*b0
	c1 = _mm256_mullo_epi16 (a0, b1);
	temp = _mm256_mullo_epi16 (a1, b0);
	c1 = _mm256_add_epi16(c1, temp);

	// c2 = a0*b2+a1*b1+a2*b0
	c2 = _mm256_mullo_epi16 (a0, b2);
	temp = _mm256_mullo_epi16 (a1, b1);
	c2 = _mm256_add_epi16(c2, temp);
	temp = _mm256_mullo_epi16 (a2, b0);
	c2 = _mm256_add_epi16(c2, temp);




	//c3 = a0*b3+a1*b2+a2*b1+a3*b0
	c3 = _mm256_mullo_epi16 (a0, b3);
	temp = _mm256_mullo_epi16 (a1, b2);
	c3 = _mm256_add_epi16(c3, temp);
	temp = _mm256_mullo_epi16 (a2, b1);
	c3 = _mm256_add_epi16(c3, temp);
	temp = _mm256_mullo_epi16 (a3, b0);
	c3 = _mm256_add_epi16(c3, temp);


	//c4 = a1*b3+a2*b2+a3*b1
	c4 = _mm256_mullo_epi16 (a1, b3);
	temp = _mm256_mullo_epi16 (a2, b2);
	c4 = _mm256_add_epi16(c4, temp);
	temp = _mm256_mullo_epi16 (a3, b1);
	c4 = _mm256_add_epi16(c4, temp);

	//c5 = a2*b3+a3*b2
	c5 = _mm256_mullo_epi16 (a2, b3);
	temp = _mm256_mullo_epi16 (a3, b2);
	c5 = _mm256_add_epi16(c5, temp);

	//c6 = a3*b3
	c6 = _mm256_mullo_epi16 (a3, b3);
}
*/

