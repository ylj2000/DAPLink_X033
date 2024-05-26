#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "stdlib.h"
#define BUFFER_MALLOC malloc

typedef struct {
    unsigned char *data;
    unsigned int buffer_size;
    unsigned int buffer_in_cnt, buffer_out_cnt;
    unsigned int buffer_in_index, buffer_out_index;
} buffer_t;

void buffer_init(buffer_t *buffer, unsigned int len);
void buffer_reset(buffer_t *buffer);
int buffer_in(buffer_t *buffer, unsigned char *data, unsigned int len);
int buffer_out(buffer_t *buffer, unsigned char *data, unsigned int *len, unsigned int max_len);
int buffer_out_without_decrease(buffer_t *buffer, unsigned char *data, unsigned int *len, unsigned int max_len);

#endif