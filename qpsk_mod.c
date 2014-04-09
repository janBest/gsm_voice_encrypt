#include <stdio.h>
#include <malloc.h>
#include <math.h>

typedef double (*PSIGNAL_FUNC)(float i,void *input_data,int input_data_len);

#define BLOCKSIZE 1024

#define DATA_RATE 200 //载波的码率
#define SAMPLING_RATE 8000 //D/A转换采样率
#define BITS 8 // 每个采样点由几位表示
#define UNIT_TIME (1000.0f/DATA_RATE) // 每个数据脉冲的时间
#define PI 3.1415926
#define FREQUENCY 1200.0 //HZ
#define SAMPLING_INTERVAL (1000.0f/SAMPLING_RATE)
#define T (1000.0f/FREQUENCY)

#define M  16          //M为插值点数,必须为偶数；

////////////////////////////
int init_input_data(void *input_data,int *input_data_len);
void print_by_bit(void * buffer, int len);
int get_a_bit(void * buffer,int i);
double qpsk_s(float i,void *input_data,int input_data_len);
char pcm_code(int V,double impulse);
double pcm_decode(int V,char data);

void qpsk_modulate();
void qpsk_demodulate();

double sinc_interpolation(float t,const double *smp_points,const int l);
int da_simulator(void *input_data,const int input_data_len,double *smp_points);
float integral(float(*fun)(float x,double *smp_points,int l),float a,float b,int n,double *smp_points,int l);
float i_f(float t,double *smp_points,int l);
float q_f(float t,double *smp_points,int l);

///////////////////////////

float test_i(float a,float b,int n,void *in,int len);
float test_q(float a,float b,int n,void *in,int len);



void main()
{



	char ch ;
	printf("modulate or demodulate?\n");
	scanf("%c",&ch);
	if(ch == 'm')
	{
		qpsk_modulate();
	}
	else if(ch == 'd')
	{
		qpsk_demodulate();
	}

}

void qpsk_modulate()
{
\
	void * input_data = NULL;
	void * output_data = NULL;
	int input_data_len = 0;
	int output_data_len = 0;
	float i=0;

	FILE *fp;
	input_data = malloc( BLOCKSIZE );
	fp = fopen("r.pcm","wb");

	if(fp == NULL)
	{
		printf("can not open the file!\n");
		return;
	}
	init_input_data(input_data,&input_data_len);
 	output_data = malloc( (int)(input_data_len * UNIT_TIME / SAMPLING_INTERVAL) + 1) ;

	//AD
	ad_simulator(qpsk_s,input_data,&input_data_len,output_data,&output_data_len);

	fwrite(output_data,1,output_data_len,fp);


	free(input_data);
	free(output_data);
	fclose(fp);
	
}



void qpsk_demodulate()
{
	///////////////////
	char *pdata;
	float a;
	float b;
	float T1;
	float T2;
	void *test_data;
	int test_data_len;
	FILE *fp;
	FILE *fp2;
	FILE *fp3;
	///////////////////
	void * input_data = NULL;
	void * output_data = NULL;
	int input_data_len = 0;
	int output_data_len = 0;
	double *smp_points;
	float t=0;	
	input_data = malloc( BLOCKSIZE * 20);
	//////////////////////////////////
	test_data = malloc(BLOCKSIZE);
	init_input_data(test_data,&test_data_len);
	/////////////////////////////////
	fp = fopen("r.pcm","rb");
	fp2 = fopen("p1.txt","w");
	fp3 = fopen("p2.txt","w"); 
	if(fp == NULL)
	{
		printf("can not open the file!\n");
		return;
	}

	input_data_len = fread(input_data,1,BLOCKSIZE * 20,fp);
	pdata=(char *)input_data;
	smp_points =(double *) malloc( sizeof(double) * input_data_len);
	da_simulator(input_data,input_data_len,smp_points);
/*
	for(t=0;t < UNIT_TIME*2;t+= SAMPLING_INTERVAL/8)
	{
		if((t/SAMPLING_INTERVAL) == ((int)t/SAMPLING_INTERVAL))
		{
		fprintf(fp2,"%f %f =====\n",t,qpsk_s(t,test_data,test_data_len));
		fprintf(fp3,"%f %f =====\n",t,sinc_interpolation(t,smp_points,input_data_len));
		}
		else
		{
		fprintf(fp2,"%f %f \n",t,qpsk_s(t,test_data,test_data_len));
		fprintf(fp3,"%f %f \n",t,sinc_interpolation(t,smp_points,input_data_len));
		}
	}
*/

	
	for(t=0;t < (input_data_len-1)*SAMPLING_INTERVAL;t+= UNIT_TIME*2)
	{

		T1 = t; 
		T2 = T1+T;
//		a = test_i(T1,T2,100,test_data,test_data_len);
//		b = test_q(T1,T2,100,test_data,test_data_len);
		a = (2*integral(i_f,T1,T2,100,smp_points,input_data_len))/T;
		b = (2*integral(q_f,T1,T2,100,smp_points,input_data_len))/T;
//		printf("%f %f\n",a,b);
//		fprintf(fp2,"%f %f %f\n",sinc_interpolation(T1,smp_points,input_data_len),sinc_interpolation(T2,smp_points,input_data_len),sinc_interpolation((T2+T2)/2,smp_points,input_data_len));
//		fprintf(fp2,"%f %f %f\n",qpsk_s(T1,test_data,test_data_len),qpsk_s(T2,test_data,test_data_len),qpsk_s((T2+T1)/2,test_data,test_data_len));
		
		if(a>0 && b>0)
		{
			printf("00");
		}
		else if(a<0&&b>0)
		{
			printf("01");
		}
		else if(a<0&&b<0)
		{
			printf("11");
		}
		else if(a>0&&b<0)
		{
			printf("10");
		}		
	}
	
	fclose(fp);
	fclose(fp2);
	fclose(fp3);
}


float test_i(float a,float b,int n,void *in,int len)
{
	float s,h,y;   
	int i;   
	s=(qpsk_s(a,in,len)*cos(2*PI*FREQUENCY*a/1000)+qpsk_s(b,in,len)*cos(2*PI*FREQUENCY*b/1000))/2;   
	h=(b-a)/n; /*积分步长*/   
	for(i=1;i<n;i++)    
		s=s+qpsk_s(a+i*h,in,len)*cos(2*PI*FREQUENCY*(a+i*h)/1000);   
	y=(2*s*h)/T;   
	return y;/*返回积分值*/  
}

float test_q(float a,float b,int n,void *in,int len)
{
	float s,h,y;   
	int i;   
	s=(qpsk_s(a,in,len)*(-sin(2*PI*FREQUENCY*a/1000))+qpsk_s(b,in,len)*(-sin(2*PI*FREQUENCY*b/1000)))/2;   
	h=(b-a)/n; /*积分步长*/   
	for(i=1;i<n;i++)    
		s=s+qpsk_s(a+i*h,in,len)*(-sin(2*PI*FREQUENCY*(a+i*h)/1000));   
	y=(2*s*h)/T;   
	return y;/*返回积分值*/  
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
int ad_simulator(PSIGNAL_FUNC s,void *input_data,const int* input_data_len,void *output_data,int *output_data_len)
{
	FILE *fp2;
	char *pout = (char *)output_data;
	float i = 0;
	int len = 0;
	char tmp ;
	fp2 = fopen("m.txt","w");
	//采样
	for(i =0; i<(*input_data_len) * UNIT_TIME; i+= SAMPLING_INTERVAL)
	{
		printf("%d %lf\n",len,s(i,input_data,*input_data_len));
		fprintf(fp2,"%d %lf\n",len,s(i,input_data,*input_data_len));
		tmp = pcm_code(1,s(i,input_data,*input_data_len));
		pout[len++]=tmp;
	}
	*output_data_len = len;
	fclose(fp2);
	return len;
}

//
int da_simulator(void *input_data,const int input_data_len,double *smp_points)
{
	FILE *fp;
	int i=0;
	char *pdata = (char*)input_data;
	fp = fopen("d.txt","w");
	for(i = 0;i < input_data_len;i+= 1)
	{
		smp_points[i]=pcm_decode(1,pdata[i]);
	
		fprintf(fp,"%d %f\n",i,smp_points[i]);
	}
	fclose(fp);
	return input_data_len;	
}


//模拟信号函数
double qpsk_s(float i,void *input_data,int input_data_len)
{
	double phase_map[2][2]={{PI/4,3*PI/4},{7*PI/4,5*PI/4}};
	int i1 = ((int)(i/(UNIT_TIME*2))) *2;
	int i2 = ((int)(i/(UNIT_TIME*2))) *2+1;
	

	int a = get_a_bit((void*)input_data,i1);
	int b = get_a_bit((void*)input_data,i2);
	

	return cos(2*PI*FREQUENCY*i/1000+phase_map[a][b]);
}


int init_input_data(void *input_data,int *input_data_len)
{
	int * pdata = (int *)input_data;
	pdata[0]=0x12345680;
	pdata[1]=0x00000000; 
	pdata[2]=0xffffffff;
	pdata[3]=0x00000000;
	pdata[4]=0xffffffff;
	pdata[5]=0x00000000;
	pdata[6]=0xffffffff;
	pdata[7]=0x000000ff;
	*input_data_len = 8*32;
	return *input_data_len;
}

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
//	printf("%d %d\n",i,( tmp >> d )&1);
	return ( tmp >> (7-d) )&1;
}

char pcm_code(int V,double impulse)   
{   
	char data = 0;
    int DL;                                          //段落区间    
	int DN;                                              //段内区间 
	int num;
	impulse=2048/V*impulse;                                //归“一”化；    
	num=(int)abs(impulse);   
   
    if(impulse>=0)   //符号位 
		data=data|1;

    //判断段落和段内区间    
    if(0<=num&&num<16)            {       DL=0;       DN=num/1;           }   
    if(16<=num&&num<32)           {       DL=1;       DN=(num-16)/1;      }   
    if(32<=num&&num<64)           {       DL=2;       DN=(num-32)/2;      }   
    if(64<=num&&num<128)      {       DL=3;       DN=(num-64)/4;      }   
    if(128<=num&&num<256)     {       DL=4;       DN=(num-128)/8;     }   
    if(256<=num&&num<512)     {       DL=5;       DN=(num-256)/16;    }   
    if(512<=num&&num<1024)        {       DL=6;       DN=(num-512)/32;        }   
    if(1024<=num&&num<2048)       {       DL=7;       DN=(num-1024)/64;    }   
	data = (data<<3)|DL;
	data = (data<<4)|DN;

	return data; 
}   
 
  
 
double pcm_decode(int V,char data)
{
	int a=0;
	double mc;
	//所在段落   
	int DL=get_a_bit((void *)&data,1)*4 + get_a_bit((void *)&data,2)*2 + get_a_bit((void *)&data,3)+1;
    //所在段内 
	int DN=get_a_bit((void *)&data,4)*8 + get_a_bit((void *)&data,5)*4 + get_a_bit((void *)&data,6)*2+ get_a_bit((void *)&data,7) +1;
	switch(DL)   
    {   
      case 1:      a=1;       mc=0+a*(DN-0.5);break;   
      case 2:      a=1;       mc=16+a*(DN-0.5);break;   
      case 3:      a=2;       mc=32+a*(DN-0.5);  break;   
      case 4:      a=4;       mc=64+a*(DN-0.5);  break;   
      case 5:      a=8;       mc=128+a*(DN-0.5); break;   
      case 6:      a=16;      mc=256+a*(DN-0.5); break;   
      case 7:      a=32;      mc=512+a*(DN-0.5); break;   
      case 8:      a=64;      mc=1024+a*(DN-0.5); break;   
    }
	 mc=mc*V/2048;   
    if(get_a_bit((void *)&data,0) ==0)  mc=mc*(-1);
	return mc;
}


float integral(float(*fun)(float x,double *smp_points,int l),float a,float b,int n,double *smp_points,int l)  
{
	float s,h,y;   
	int i;   
	s=(fun(a,smp_points,l)+fun(b,smp_points,l))/2;   
	h=(b-a)/n; /*积分步长*/   
	for(i=1;i<n;i++)    
		s=s+fun(a+i*h,smp_points,l);   
	y=s*h;   
	return y;/*返回积分值*/  
}

float i_f(float t,double *smp_points,int l)
{
	return cos(2*PI*FREQUENCY*t/1000)*sinc_interpolation(t,smp_points,l);
}

float q_f(float t,double *smp_points,int l)
{
	return -sin(2*PI*FREQUENCY*t/1000)*sinc_interpolation(t,smp_points,l);
}



