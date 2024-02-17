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
#include "errno.h"

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

int _put_integer(row_list_t* row_list, size_t index, const char* field, uint64_t data, const char* file, size_t line);
#define put_integer(row_list, index, field, data) _put_integer(row_list, index, field, data, __FILE__, __LINE__)

int _put_string(row_list_t* row_list, size_t index, const char* field, const char* data, const char* file, size_t line);
#define put_string(row_list, index, field, data) _put_string(row_list, index, field, data, __FILE__, __LINE__)

int _get_integer(row_list_t* row_list, size_t index, const char* field, uint64_t* data, const char* file, size_t line);
#define get_integer(row_list, index, field, data) _get_integer(row_list, index, field, data, __FILE__, __LINE__)

int _get_string(row_list_t* row_list, size_t index, const char* field, char* data, size_t data_size, const char* file, size_t line);
#define get_string(row_list, index, field, data, data_size) _get_string(row_list, index, field, data, data_size, __FILE__, __LINE__)

void dump_rows(row_list_t* row_list);

int _insert_rows(row_list_t* row_list, const char* file, size_t line);
#define insert_rows(row_list) _insert_rows(row_list, __FILE__, __LINE__)

int _load_rows(row_list_t* row_list, table_metadata_t* table, const char* file, size_t line);
#define load_rows(row_list, table) _load_rows(row_list, table, __FILE__, __LINE__)

int _add_row(row_list_t* row_list, const char* file, size_t line);
#define add_row(row_list) _add_row(row_list, __FILE__, __LINE__);

#endif // __ROW__