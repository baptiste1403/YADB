#ifndef __BLOCK__
#define __BLOCK__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "logger.h"

#define BLOCK_SIZE 512

typedef uint16_t tuple_ptr_t;

typedef struct {
    uint16_t nb_rows;
    uint16_t row_ptr_end;
    uint16_t row_end;
} block_header_t;

#define BLOCK_BODY (BLOCK_SIZE - sizeof(block_header_t))

typedef struct {
    block_header_t header;
    char data[BLOCK_BODY];
} block_t;

typedef struct {
    uint16_t size;
} tuple_header_t;

typedef struct {
    tuple_header_t header;
    char* data;
} tuple_view_t;

#define block_ptr_to_data_index(data_ptr) (data_ptr) - sizeof(block_header_t)

int block_put_tuple(block_t* block, const char* tuple, size_t tuple_size);
int block_get_tuple_view(block_t* block, size_t index, tuple_view_t* res);
size_t block_get_remain(const block_t* block);
void block_reset(block_t* block);

#endif // __BLOCK__