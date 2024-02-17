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

table_metadata_t* _create_table(database_t* db, char* name, const char* file, size_t line);
#define create_table(db, name) _create_table(db, name, __FILE__, __LINE__)

table_metadata_t* get_table(database_t* db, const char* name);

int _save_database(database_t* database, const char* file, size_t line);
#define save_database(database) _save_database(database, __FILE__, __LINE__)

int _load_database(database_t* database, const char* file, size_t line);
#define load_database(database) _load_database(database, __FILE__, __LINE__)

void free_database(database_t* database);

#endif // __DATABASE__