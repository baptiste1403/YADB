#ifndef __DATABASE__
#define __DATABASE__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "logger.h"
#include "errno.h"
#include "list.h"
#include "defer.h"

#define TABLE_METADATA_FILE "tables_metadatas.yadb"

typedef struct {
    size_t count;
    size_t capacity;
    table_metadata_t* items;
} database_t;

table_metadata_t* create_table(database_t* db, char* name);
table_metadata_t* get_table(database_t* db, const char* name);
int save_database(database_t* database);
int load_database(database_t* database);
void free_database(database_t* database);

#endif // __DATABASE__