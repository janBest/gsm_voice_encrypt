#ifndef MODULATE_H
#define MODULATE_H

#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <malloc.h>
#include "bitmap.h"
typedef double (*PSIGNAL_FUNC)(float i,void *input_data,int input_data_len);

#define BLOCKSIZE 1024
#define N 24 //子载波数量
#define DATA_RATE 2400.0 //载波的码率
#define SAMPLING_RATE 8000 //D/A转换采样率
#define BITS 8 // 每个采样点由几位表示
#define UNIT_TIME (1000.0/DATA_RATE) // 每个数据脉冲的时间
#define PI 3.1415926
#define FREQUENCY 50 //HZ 
#define SAMPLING_INTERVAL (1000.0f/SAMPLING_RATE)
#define T (1000.0f/FREQUENCY)
#define DEPTH 2 //BYTE

#define M  16          //M为插值点数,必须为偶数；

#define SYM_PERIOD (2*N*UNIT_TIME)
#define SYM_PERIOD_SMP (SYM_PERIOD/SAMPLING_INTERVAL + 1)

#define IN_BUFSIZ (N*2)
#define OUT_BUFSIZ SYM_PERIOD_SMP * DEPTH


double qpsk_s(float i,void *input_data,int input_data_len);


void ofdm_modulate_frame(void *in_buf,void *out_buf);
void ofdm_demodulate_frame(void *out_buf,void *in_buf);

void ad_simulator(PSIGNAL_FUNC s,void *in_buf,void *out_buf);
void da_simulator(void *input_data,double *smp_points);

double sinc_interpolation(float t,const double *smp_points,const int l);

float integral(float(*fun)(float t,int m,double *smp_points,int l),float a,float b,int n,int m,double *smp_points,int l);
float i_f(float t,int m,double *smp_points,int l);
float q_f(float t,int m,double *smp_points,int l);
short lpcm16_code(int min_v,int max_v,double data);
double lpcm16_decode(int min_v,int max_v,short data);




#endif