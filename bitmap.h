#ifndef BITMAP_H
#define BITMAP_H


void print_by_bit(void * buffer, int len);
int get_a_bit(const void * buffer,int i);
void set_a_bit(void * buffer,int i,int b);
double compute_error_rate(void * dest_data, void *source_data, int len);


#endif 