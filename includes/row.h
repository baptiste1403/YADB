#ifndef __ROW__
#define __ROW__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "table.h"
#include "field.h"
#include "logger.h"
#include "list.h"
#include "defer.h"

#define TABLE_NAME_SIZE 30

typedef struct {
    char* data;
} row_t;

typedef struct {
    table_metadata_t* table;
    row_t* items;
    size_t count;
    size_t capacity;
} row_list_t;


void free_row_list(row_list_t* row_list);
int put_integer(row_list_t* row, size_t index, const char* field, uint64_t data);
int put_string(row_list_t* row, size_t index, const char* field, const char* data);
int get_integer(row_list_t* row, size_t index, const char* field, uint64_t* data);
int get_string(row_list_t* row, size_t index, const char* field, char* data, size_t data_size);
void dump_rows(row_list_t* row_list);
int insert_rows(row_list_t* row_list);
int load_rows(row_list_t* row_list, table_metadata_t* table);
int add_row(row_list_t* row_list);

#endif // __ROW__