#include "table.h"

int add_field(table_metadata_t* table, const char* name, type_t type, size_t size) {

    field_metadata_t field_metadata = {0};
    strncpy(field_metadata.name, name, TABLE_NAME_SIZE);
    field_metadata.offset = table->row_size;
    field_metadata.size = size;
    field_metadata.type = type;
    table->row_size += size;
    list_append(table, field_metadata);
    return 0;
}

int get_field_index(table_metadata_t* table, const char* field) {
    for(size_t i = 0; i < table->count; i++) {
        if(strcmp(table->items[i].name, field) == 0) {
            return (int)i;
        }
    }
    return -1;
}

void free_table(table_metadata_t* table) {
    if(table->items != NULL) {
        free(table->items);
        *table = (table_metadata_t){0};
    }
}

int save_table_metadata(table_metadata_t* table, FILE* fd) {
    if(table->count == 0) {
        log_message(LOG_ERROR, "table '%s' is empty", table->name);
        return -1;
    }
    // write metadata for table
    if(fwrite(table->name, TABLE_NAME_SIZE, 1, fd) != 1) {
        log_message(LOG_ERROR,  "cannot write table name\n");
        return -1;
    }

    if(fwrite(&table->count, sizeof(table->count), 1, fd) != 1) {
        log_message(LOG_ERROR,  "cannot write table count\n");
        return -1;
    }

    if(fwrite(&table->row_size, sizeof(table->row_size), 1, fd) != 1) {
        log_message(LOG_ERROR,  "cannot write table row_size\n");
        return -1;
    }

    // write items metadata
    for(size_t i = 0; i < table->count; i++) {
        if(fwrite(&table->items[i], sizeof(field_metadata_t), 1, fd) != 1) {
            log_message(LOG_ERROR,  "cannot write table field metadata\n");
            return -1;
        }
    }
    return 0;
}

int load_table_metadata(table_metadata_t* table, FILE* fd) {
    int defer_return_value = 0;
    if(table->count != 0) {
        log_message(LOG_ERROR, "table destination is not empty");
        return_defer(-1);
    }
    // read metadata for table
    if(fread(table->name, TABLE_NAME_SIZE, 1, fd) != 1) {
        log_message(LOG_ERROR,  "cannot read table name\n");
        return_defer(-1);
    }

    size_t nb_fields;
    if(fread(&nb_fields, sizeof(nb_fields), 1, fd) != 1) {
        log_message(LOG_ERROR,  "cannot read table count\n");
        return_defer(-1);
    }

    if(fread(&table->row_size, sizeof(table->row_size), 1, fd) != 1) {
        log_message(LOG_ERROR,  "cannot read table row_size\n");
        return_defer(-1);
    }

    // read items metadata
    for(size_t i = 0; i < nb_fields; i++) {
        field_metadata_t field = {0};
        if(fread(&field, sizeof(field_metadata_t), 1, fd) != 1) {
            log_message(LOG_ERROR,  "cannot read table field metadata\n");
            return_defer(-1);
        }
        list_append(table, field);
    }

    
    defer:
        if(defer_return_value == -1) {
            free_table(table);
            *table = (table_metadata_t){0};
        }
        return defer_return_value;
}

void dump_table_metadata(table_metadata_t* table) {
    printf("table '%s' (\n", table->name);
    for(size_t i = 0; i < table->count; i++) {
        printf("   %s(%lu) %s\n", to_string_type(table->items[i].type), table->items[i].size, table->items[i].name);
    }
    puts(")");
}