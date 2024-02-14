#ifndef __TABLE__
#define __TABLE__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "field.h"

#define TABLE_NAME_SIZE 30

typedef struct {
    char name[TABLE_NAME_SIZE];
    field_metadata_t* fields;
    size_t row_size;
    size_t nb_fields;
} table_metadata_t;

int get_field_index(table_metadata_t* table, const char* field);
void free_table(table_metadata_t* table);
int write_table_metadata(table_metadata_t* table, FILE* fd);
int read_table_metadata(table_metadata_t* table, FILE* fd);
void dump_table_metadata(table_metadata_t* table);

#endif // __TABLE__