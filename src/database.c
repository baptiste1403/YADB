#include "database.h"

void init_table(table_builder_t* builder, const char* name, size_t nb_fields) {
    builder->table = (table_metadata_t*)malloc(sizeof(table_metadata_t));
    *builder->table = (table_metadata_t){0};
    strncpy(builder->table->name, name, TABLE_NAME_SIZE);
    builder->table->nb_fields = nb_fields;
    builder->table->fields = (field_metadata_t*)malloc(sizeof(field_metadata_t)*nb_fields);
    builder->table->row_size = 0;
    builder->nb_field_set = 0;
}

int set_field(table_builder_t* builder, const char* name, type_t type, size_t size) {
    if(builder->table->nb_fields == builder->nb_field_set) {
        fprintf(stderr, "Cannot add field to table '%s' because it's full\n", builder->table->name);
        return -1;
    }
    field_metadata_t* field_metadata = &builder->table->fields[builder->nb_field_set];
    strncpy(field_metadata->name, name, TABLE_NAME_SIZE);
    field_metadata->offset = builder->table->row_size;
    field_metadata->size = size;
    field_metadata->type = type;
    builder->table->row_size += size;
    builder->nb_field_set++;
    return 0;
}

int get_table(table_builder_t* builder, table_metadata_t* dest) {
    if(builder->table == NULL || builder->nb_field_set < builder->table->nb_fields) {
        fprintf(stderr, "Uninitialized table, cannot return table that are not fully set\n");
        return -1;
    }
    *dest = *builder->table;
    return 0;
}

int save_database(database_t* database) {

    FILE* fd = fopen(TABLE_METADATA_FILE, "w");
    if (fd == NULL) {
        fprintf(stderr, "cannot open file\n");
        return -1;
    }

    if(fwrite(&database->nb_table, sizeof(database->nb_table), 1, fd) != 1) {
        fprintf(stderr, "Cannot write table number\n");
    }
    
    for(size_t i = 0; i < database->nb_table; i++) {
        if(write_table_metadata(&database->tables[i], fd) < 0) {
            fprintf(stderr, "Cannot write table metadata\n");
        }
        free(database->tables[i].fields);
    }

    free(database->tables);
    fclose(fd);
    return 0;
}

int load_database(database_t* database) {

    FILE* fd = fopen(TABLE_METADATA_FILE, "r");
    if (fd == NULL) {
        fprintf(stderr, "cannot open file\n");
        return -1;
    }

    if(fread(&database->nb_table, sizeof(database->nb_table), 1, fd) != 1) {
        fprintf(stderr, "Cannot write table number\n");
    }

    database->tables = (table_metadata_t*)malloc(sizeof(table_metadata_t)*database->nb_table);
    
    for(size_t i = 0; i < database->nb_table; i++) {
        if(read_table_metadata(&database->tables[i], fd) < 0) {
            fprintf(stderr, "Cannot write table metadata\n");
        }
    }

    fclose(fd);
    return 0;

}