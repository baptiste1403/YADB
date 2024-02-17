#include "database.h"

table_metadata_t* _create_table(database_t* db, char* name, const char* file, size_t line) {
    if(get_table(db, name) != NULL) {
        _log_message(LOG_ERROR, file, line, "table '%s' already exist", name);
        return NULL;
    }
    table_metadata_t table = {0};
    strncpy(table.name, name, TABLE_NAME_SIZE);
    list_append(db, table);
    return &db->items[db->count-1];
}

table_metadata_t* get_table(database_t* db, const char* name) {
    for(size_t i = 0; i < db->count; i++) {
        if(strcmp(db->items[i].name, name) == 0) {
            return &db->items[i];
        }
    }
    return NULL;
}

void free_database(database_t* database) {
    if(database->count > 0) {
        for(size_t i = 0; i < database->count; i++) {
            free_table(&database->items[i]);
        }
        free(database->items);
        *database = (database_t){0};
    }
}

int _save_database(database_t* database, const char* file, size_t line) {
    int defer_return_value = 0;
    if(database->count == 0) {
        _log_message(LOG_ERROR, file, line, "database is empty");
        return -1;
    }
    FILE* fd = fopen(TABLE_METADATA_FILE, "w");
    if (fd == NULL) {
        _log_message(LOG_ERROR, file, line, "cannot open file");
        return_defer(-1);
    }

    if(fwrite(&database->count, sizeof(database->count), 1, fd) != 1) {
        _log_message(LOG_ERROR, file, line, "Cannot write table number");
        return_defer(-1);
    }
    
    for(size_t i = 0; i < database->count; i++) {
        if(_save_table_metadata(&database->items[i], fd, file, line) < 0) {
            _log_message(LOG_ERROR, file, line, "Cannot write table metadata");
            return_defer(-1);
        }
    }

    defer:
        fclose(fd);
        return defer_return_value;
}

int _load_database(database_t* database, const char* file, size_t line) {
    int defer_return_value = 0;
    if(database->count != 0) {
        _log_message(LOG_ERROR, file, line, "database destination is not empty");
        return -1;
    }
    FILE* fd = fopen(TABLE_METADATA_FILE, "r");
    if (fd == NULL) {
        _log_message(LOG_ERROR, file, line, "cannot open file");
        return_defer(-1);
    }

    size_t nb_tables;
    if(fread(&nb_tables, sizeof(nb_tables), 1, fd) != 1) {
        _log_message(LOG_ERROR, file, line, "Cannot write table number");
        return_defer(-1);
    }
    
    for(size_t i = 0; i < nb_tables; i++) {
        table_metadata_t table = {0};
        if(_load_table_metadata(&table, fd, file, line) < 0) {
            _log_message(LOG_ERROR, file, line, "Cannot write table metadata");
            return_defer(-1);
        }
        list_append(database, table);
    }
    
    defer:
        if(defer_return_value == -1) {
            free_database(database);
            *database = (database_t){0};
        }
        fclose(fd);
        return defer_return_value;
}