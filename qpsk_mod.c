#include <stdio.h>
#include <malloc.h>
#include "modulate.h"

void read_data(void * buf);
void test_modulate(void * in_buf,void * out_buf,FILE *fp);
void test_demodulate(void * out_buf,void * in_buf,FILE *fp);

void main()
{
	FILE *fp;
	char in_buf[(int)IN_BUFSIZ];
	char out_buf[(int)OUT_BUFSIZ];

	char ch ;
	int i=0;
	memset(in_buf,0,(int)IN_BUFSIZ);
	memset(out_buf,0,(int)OUT_BUFSIZ);

	printf("modulate or demodulate?\n");
	scanf("%c",&ch);
	if(ch == 'm')
	{
		fp = fopen("r.raw","wb");
		test_modulate(in_buf,out_buf,fp);
		fclose(fp);
	}
	else if(ch == 'd')
	{
		
		fp = fopen("r2.raw","rb");
		test_demodulate(out_buf,in_buf,fp);
		fclose(fp);
	}

}

void test_modulate(void * in_buf,void * out_buf,FILE *fp)
{
	int i=0;
		for(i = 0; i<128;i++)
		{
			read_data(in_buf);
			ofdm_modulate_frame(in_buf,out_buf);
			fwrite(out_buf,(int)OUT_BUFSIZ,1,fp);
		}
}
void test_demodulate(void * out_buf,void * in_buf,FILE *fp)
{
	int i=0;
	char test_buf[(int)IN_BUFSIZ];
	float sum = 0;
	memset(test_buf,0,(int)IN_BUFSIZ);
		for(i = 0;i<128;i++)
		{
			fread(out_buf,OUT_BUFSIZ,1,fp);
			ofdm_demodulate_frame(out_buf,in_buf);
			read_data(test_buf);
	//		printf("\nResult Data");
	//		print_by_bit(in_buf,(int)IN_BUFSIZ);
	//		printf("\n\n");
	//		printf("Original Data");
	//		print_by_bit(test_buf, (int)IN_BUFSIZ);
			sum += compute_error_rate(in_buf, test_buf,(int)IN_BUFSIZ);
			printf("\n\nError Rate:\n%lf\n", compute_error_rate(in_buf, test_buf,(int)IN_BUFSIZ));
		}
		printf("Total %lf\n",sum/i);
}

void read_data(void * buf)
{
	int * pbuf =(int *) buf;
	pbuf[0] = 0x1234ff8f;
	pbuf[1] = 0x1234f99f;
}




