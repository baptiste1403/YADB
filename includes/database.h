#ifndef __DATABASE__
#define __DATABASE__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"

#define TABLE_METADATA_FILE "tables_metadatas.yadb"

typedef struct {
    size_t nb_table;
    table_metadata_t* tables;
} database_t;

typedef struct {
    table_metadata_t* table;
    size_t nb_field_set;
} table_builder_t;

void init_table(table_builder_t* builder, const char* name, size_t nb_fields);
int set_field(table_builder_t* builder, const char* name, type_t type, size_t size);
int get_table(table_builder_t* builder, table_metadata_t* dest);
int save_database(database_t* database);
int load_database(database_t* database);

#endif // __DATABASE__