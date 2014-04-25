#include "modulate.h"

//调制一个符号周期的数据
void ofdm_modulate_frame(void *in_buf,void *out_buf)
{


 
	//AD
	ad_simulator(qpsk_s,in_buf,out_buf);
//	ad_simulator(qpsk_test,input_data,&input_data_len,output_data,&output_data_len);

}



void ofdm_demodulate_frame(void *out_buf,void *in_buf)
{
	///////////////////
	float a;
	float b;
	float T1;
	float T2;
	int bit_num = 0;
	///////////////////

	double *smp_points;
	float t=0;
	int m = 0;




	smp_points =(double *) malloc( sizeof(double) * (int)SYM_PERIOD_SMP );
	da_simulator(out_buf,smp_points);
	
	
	for(t=0;t < SYM_PERIOD ;t+= SYM_PERIOD)
	{

		T1 = t; 
		T2 = T1+T;
		for(m = 1;m <= N; m++)
		{
			a = (2*integral(i_f,T1,T2,100,m,smp_points, (int)SYM_PERIOD_SMP))/T;
			b = (2*integral(q_f,T1,T2,100,m,smp_points, (int)SYM_PERIOD_SMP))/T;
			
	//		printf("%f %f\n",a,b);	
			if(a>0 && b>0)
			{
				set_a_bit(in_buf,bit_num,0);
				set_a_bit(in_buf,bit_num+1,0);
				//printf("00");
			}
			else if(a<0&&b>0)
			{
				set_a_bit(in_buf,bit_num,0);
				set_a_bit(in_buf,bit_num+1,1);
				//printf("01");
			}
			else if(a<0&&b<0)
			{
				set_a_bit(in_buf,bit_num,1);
				set_a_bit(in_buf,bit_num+1,1);
				//printf("11");
			}
			else if(a>0&&b<0)
			{
				set_a_bit(in_buf,bit_num,1);
				set_a_bit(in_buf,bit_num+1,0);
				//printf("10");
			}
			bit_num += 2;
		}
	
	}
	/*
	init_input_data(input_data, &input_data_len);
	printf("\nResult Data");
	print_by_bit(output_data,bit_num);
	printf("\n\n");
	printf("Original Data");
	print_by_bit(input_data, bit_num);
	printf("\n\nError Rate:\n%lf\n", compute_error_rate(input_data, output_data, bit_num));
	*/
}







double sinc_interpolation(float t,const double *smp_points,const int l)
{

	int n = (int) (t/SAMPLING_INTERVAL);
	int a = M/2 - 1;
	double res = 0;
	int j=0;
	double temp = 0;
	if((int) (t/SAMPLING_INTERVAL) == (t/SAMPLING_INTERVAL) )
	{
		return smp_points[n];
	}
	for(j=n-a;j<(n+a+2);j++)
	{
		if(j < 0)
			continue;
		if(j >= l)
			continue;
		temp=sin(((t-j*SAMPLING_INTERVAL)*PI)/SAMPLING_INTERVAL)/((PI*(t-j*SAMPLING_INTERVAL)/SAMPLING_INTERVAL));
		res+=smp_points[j]*temp;
	}
	return res;
}


//模拟AD转换器


void ad_simulator(PSIGNAL_FUNC s,void *in_buf,void *out_buf)
{

	FILE *fp2;
	short *pout = (short *)out_buf;
	float i = 0;
	int sym_point = 0;
	short tmp ;
	fp2 = fopen("m.txt","w");

	//采样
	for(i =0; i<= SYM_PERIOD; i+= SAMPLING_INTERVAL)
	{
		tmp = lpcm16_code(-N,N,s(i,in_buf,IN_BUFSIZ));
		pout[sym_point++]=tmp;

		printf("%d %lf %d\n",sym_point,s(i,in_buf,IN_BUFSIZ),tmp);
		fprintf(fp2,"%d %lf %d\n",sym_point,s(i,in_buf,IN_BUFSIZ),(unsigned char)tmp);

	}

	fclose(fp2);
}

short lpcm16_code(int min_v,int max_v,double data)
{
	short max_d = 32767;
	short min_d = -32768;
	short res = min_d + (short)((data - min_v) * (max_d - min_d) / (max_v - min_v));
	return res;
}

double lpcm16_decode(int min_v,int max_v,short data)
{
	short max_d = 32767;
	short min_d = -32768;
	double res = min_v + (data - min_d) * ((double)(max_v - min_v) / (max_d - min_d));
	return res;
}


void da_simulator(void *input_data,double *smp_points)
{
	FILE *fp;
	int i=0;
	short *pdata = (short*) input_data;
	fp = fopen("d.txt","w");
	for(i = 0;i < SYM_PERIOD_SMP ;i+= 1)
	{
		smp_points[i]=lpcm16_decode(-N,N,pdata[i]);
	
		fprintf(fp,"%d %f\n",i,smp_points[i]);
	}
	fclose(fp);
}


//模拟信号函数
double qpsk_s(float t,void *input_data,int input_data_len)
{
	double phase_map[2][2]={{PI/4,3*PI/4},{7*PI/4,5*PI/4}};
	double res = 0;
	int m = (int)(t/(UNIT_TIME*2*N));
	int i1 = 0;
	int i2 = 0;
	int a = 0;
	int b = 0;
	int i = 0;
	for(i = 0;i < N;i++)
	{
		i1 = 2*N*m+2*i;
		i2 = i1+1;
	

		a = get_a_bit((void*)input_data,i1);
		b = get_a_bit((void*)input_data,i2);
		res += cos(2*PI*FREQUENCY*(i+1)*t/1000+phase_map[a][b]);

	}

	return res;
}


float integral(float(*fun)(float t,int m,double *smp_points,int l),float a,float b,int n,int m,double *smp_points,int l)  
{
	float s,h,y;   
	int i;   
	s=(fun(a,m,smp_points,l)+fun(b,m,smp_points,l))/2;   
	h=(b-a)/n; /*积分步长*/   
	for(i=1;i<n;i++)    
		s=s+fun(a+i*h,m,smp_points,l);   
	y=s*h;   
	return y;/*返回积分值*/  
}

float i_f(float t,int m,double *smp_points,int l)
{
	return cos(2*PI*FREQUENCY*m*t/1000)*sinc_interpolation(t,smp_points,l);
}

float q_f(float t,int m,double *smp_points,int l)
{
	return -sin(2*PI*FREQUENCY*m*t/1000)*sinc_interpolation(t,smp_points,l);
}
