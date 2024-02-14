#include "table.h"

int get_field_index(table_metadata_t* table, const char* field) {
    for(size_t i = 0; i < table->nb_fields; i++) {
        if(strcmp(table->fields[i].name, field) == 0) {
            return (int)i;
        }
    }
    return -1;
}

void free_table(table_metadata_t* table) {
    free(table->fields);
}

int write_table_metadata(table_metadata_t* table, FILE* fd) {
    // write metadata for table
    if(fwrite(table->name, TABLE_NAME_SIZE, 1, fd) != 1) {
        fprintf(stderr, "cannot write table name\n");
        return -1;
    }

    if(fwrite(&table->nb_fields, sizeof(table->nb_fields), 1, fd) != 1) {
        fprintf(stderr, "cannot write table nb_fields\n");
        return -1;
    }

    if(fwrite(&table->row_size, sizeof(table->row_size), 1, fd) != 1) {
        fprintf(stderr, "cannot write table row_size\n");
        return -1;
    }

    // write fields metadata
    for(size_t i = 0; i < table->nb_fields; i++) {
        if(fwrite(&table->fields[i], sizeof(field_metadata_t), 1, fd) != 1) {
            fprintf(stderr, "cannot write table field metadata\n");
            return -1;
        }
    }
    return 0;
}

int read_table_metadata(table_metadata_t* table, FILE* fd) {
    // read metadata for table
    if(fread(table->name, TABLE_NAME_SIZE, 1, fd) != 1) {
        fprintf(stderr, "cannot read table name\n");
        return -1;
    }

    if(fread(&table->nb_fields, sizeof(table->nb_fields), 1, fd) != 1) {
        fprintf(stderr, "cannot read table nb_fields\n");
        return -1;
    }

    if(fread(&table->row_size, sizeof(table->row_size), 1, fd) != 1) {
        fprintf(stderr, "cannot read table row_size\n");
        return -1;
    }

    table->fields = (field_metadata_t*)malloc(sizeof(field_metadata_t)*table->nb_fields);

    // read fields metadata
    for(size_t i = 0; i < table->nb_fields; i++) {
        if(fread(&table->fields[i], sizeof(field_metadata_t), 1, fd) != 1) {
            fprintf(stderr, "cannot read table field metadata\n");
            return -1;
        }
    }

    return 0;
}

void dump_table_metadata(table_metadata_t* table) {
    printf("table '%s' (\n", table->name);
    for(size_t i = 0; i < table->nb_fields; i++) {
        printf("   %s(%lu) %s\n", to_string_type(table->fields[i].type), table->fields[i].size, table->fields[i].name);
    }
    puts(")");
}