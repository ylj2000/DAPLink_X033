#include "buffer.h"

void buffer_init(buffer_t *buffer, unsigned int len)
{
    buffer->data = (unsigned char *)BUFFER_MALLOC(len);
    buffer->buffer_size = len;
    buffer->buffer_in_cnt = 0;
    buffer->buffer_out_cnt = 0;
    buffer->buffer_in_index = 0;
    buffer->buffer_out_index = 0;
}

void buffer_reset(buffer_t *buffer)
{
    buffer->buffer_in_cnt = 0;
    buffer->buffer_out_cnt = 0;
    buffer->buffer_in_index = 0;
    buffer->buffer_out_index = 0;
}

int buffer_in(buffer_t *buffer, unsigned char *data, unsigned int len)
{
    if (buffer->buffer_in_cnt + len - buffer->buffer_out_cnt > buffer->buffer_size)
    {
        return -1;
    }

    unsigned int i;
    for (i = 0; i < len; i++)
    {
        buffer->data[(buffer->buffer_in_index + i) % buffer->buffer_size] = data[i];
    }
    buffer->buffer_in_index = (buffer->buffer_in_index + len) % buffer->buffer_size;
    buffer->buffer_in_cnt += len;
    return 0;
}

int buffer_out(buffer_t *buffer, unsigned char *data, unsigned int *len, unsigned int max_len)
{
    if (buffer->buffer_out_cnt >= buffer->buffer_in_cnt)
    {
        return -1;
    }

    *len = buffer->buffer_in_cnt - buffer->buffer_out_cnt;
    if (*len > max_len)
    {
        *len = max_len;
    }

    unsigned int i;
    for (i = 0; i < *len; i++)
    {
        if (data)
            data[i] = buffer->data[(buffer->buffer_out_index + i) % buffer->buffer_size];
    }
    buffer->buffer_out_index = (buffer->buffer_out_index + *len) % buffer->buffer_size;
    buffer->buffer_out_cnt += *len;
    return 0;
}

int buffer_out_without_decrease(buffer_t *buffer, unsigned char *data, unsigned int *len, unsigned int max_len)
{
    if (buffer->buffer_out_cnt >= buffer->buffer_in_cnt)
    {
        return -1;
    }

    *len = buffer->buffer_in_cnt - buffer->buffer_out_cnt;
    if (*len > max_len)
    {
        *len = max_len;
    }

    unsigned int i;
    for (i = 0; i < *len; i++)
    {
        if (data)
            data[i] = buffer->data[(buffer->buffer_out_index + i) % buffer->buffer_size];
    }
    return 0;
}
