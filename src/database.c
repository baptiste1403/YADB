#include "database.h"

table_metadata_t* create_table(database_t* db, char* name) {
    if(get_table(db, name) != NULL) {
        log_message(LOG_ERROR, "table '%s' already exist", name);
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

int save_database(database_t* database) {
    int defer_return_value = 0;
    if(database->count == 0) {
        log_message(LOG_ERROR, "database is empty");
        return -1;
    }
    FILE* fd = fopen(TABLE_METADATA_FILE, "w");
    if (fd == NULL) {
        log_message(LOG_ERROR, "cannot open file");
        return_defer(-1);
    }

    if(fwrite(&database->count, sizeof(database->count), 1, fd) != 1) {
        log_message(LOG_ERROR, "Cannot write table number");
        return_defer(-1);
    }
    
    for(size_t i = 0; i < database->count; i++) {
        if(save_table_metadata(&database->items[i], fd) < 0) {
            log_message(LOG_ERROR, "Cannot write table metadata");
            return_defer(-1);
        }
    }

    defer:
        fclose(fd);
        return defer_return_value;
}

int load_database(database_t* database) {
    int defer_return_value = 0;
    if(database->count != 0) {
        log_message(LOG_ERROR, "database destination is not empty");
        return -1;
    }
    FILE* fd = fopen(TABLE_METADATA_FILE, "r");
    if (fd == NULL) {
        log_message(LOG_ERROR, "cannot open file");
        return_defer(-1);
    }

    size_t nb_tables;
    if(fread(&nb_tables, sizeof(nb_tables), 1, fd) != 1) {
        log_message(LOG_ERROR, "Cannot write table number");
        return_defer(-1);
    }
    
    for(size_t i = 0; i < nb_tables; i++) {
        table_metadata_t table = {0};
        if(load_table_metadata(&table, fd) < 0) {
            log_message(LOG_ERROR, "Cannot write table metadata");
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