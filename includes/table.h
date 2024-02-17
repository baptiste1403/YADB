#ifndef __TABLE__
#define __TABLE__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "field.h"
#include "logger.h"
#include "list.h"
#include "defer.h"

#define TABLE_NAME_SIZE 30

typedef struct {
    char name[TABLE_NAME_SIZE];
    field_metadata_t* items;
    size_t row_size;
    size_t count;
    size_t capacity;
} table_metadata_t;

void add_field(table_metadata_t* table, const char* name, type_t type, size_t size);

int get_field_index(table_metadata_t* table, const char* field);

int _save_table_metadata(table_metadata_t* table, FILE* fd, const char* file, size_t line);
#define save_table_metadata(table, fd) _save_table_metadata(table, fd, __FILE__, __LINE__)

int _load_table_metadata(table_metadata_t* table, FILE* fd, const char* file, size_t line);
#define load_table_metadata(table, fd) _load_table_metadata(table, fd, __FILE__, __LINE__)

void dump_table_metadata(table_metadata_t* table);
void free_table(table_metadata_t* table);

#endif // __TABLE__