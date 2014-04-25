#include "bitmap.h"

void print_by_bit(void * buffer, int len)
{

	const char *pdata = (char *)buffer;
	int i=0;
	for(i=0 ;i < len;i++ )
	{
		if(i % 32 == 0)
		{
			printf("\n");
		}

		printf("%d",get_a_bit((void *)pdata,i));
	}
}

int get_a_bit(const void * buffer,int i)
{
	const char *pdata = (char *)buffer;
	char tmp = pdata[i/8];
	int d = i%8;
	return ( tmp >> (7-d) )&1;
}


void set_a_bit(void * buffer,int i,int b)
{
	char *pdata = (char *)buffer;
	int d = i%8;
	pdata[i/8]=pdata[i/8] | (b<<(7-d));

}

double compute_error_rate(void * dest_data, void *source_data,int len)
{
	const char *pdest_data = (char *)dest_data;
	const char *psource_data = (char *)source_data;
	int error = 0;
	int i = 0;
	for (i = 0; i < len; i++)
	{
		if (get_a_bit((void *)pdest_data, i) != get_a_bit((void*)psource_data, i))
			error++;
	}
	return error * 1.0 / len;
}
