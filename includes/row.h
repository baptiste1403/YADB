#ifndef __ROW__
#define __ROW__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "table.h"
#include "field.h"

#define TABLE_NAME_SIZE 30

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

typedef struct {
    table_metadata_t* table;
    char* data;
} row_t;

row_t create_table_row(table_metadata_t* table);
void free_table_row(row_t* row);
int put_integer(row_t* row, const char* field, uint64_t data);
int put_string(row_t* row, const char* field, const char* data);
int get_integer(row_t* row, const char* field, uint64_t* data);
int get_string(row_t* row, const char* field, char* data, size_t data_size);
void dump_rows(row_t* row, size_t nb_rows);

#endif // __ROW__