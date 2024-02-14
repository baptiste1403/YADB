#include "row.h"

#define ROW_DUMP_FIELD_WIDTH 20

row_t create_table_row(table_metadata_t* table) {
    row_t res = {
        .table = table,
        .data = malloc(table->row_size)
    };

    if(res.data == NULL) {
        perror("malloc error");

    }
    return res;
}

void free_table_row(row_t* row) {
    if(row->data != NULL) {
        free(row->data);
    }
}

int get_data(row_t* row, size_t field, char** data) {
    if(field >= row->table->nb_fields) {
        fprintf(stderr, "field index out of range for table '%s' with index %lu\n", row->table->name, field);
        return -1;
    }
    field_metadata_t field_metadata = row->table->fields[field];
    *data = row->data+field_metadata.offset;
    return 0;
}

int get_data_from_name(row_t* row, const char* field, char** data) {
    int index = get_field_index(row->table, field);
    if(index < 0) {
        fprintf(stderr, "field '%s' doesn't exist in table '%s'\n", field, row->table->name);
        return -1;
    }
    return get_data(row, index, data);
}

int put_data(row_t* row, size_t field, const char* data) {
    if(field >= row->table->nb_fields) {
        fprintf(stderr, "field index out of range for table '%s' with index %lu\n", row->table->name, field);
        return -1;
    }
    field_metadata_t field_metadata = row->table->fields[field];
    char* dest_ptr = row->data+field_metadata.offset;
    memcpy(dest_ptr, data, field_metadata.size);
    return 0;
}

int put_data_from_name(row_t* row, const char* field, const char* data) {
    int index = get_field_index(row->table, field);
    if(index < 0) {
        fprintf(stderr, "field '%s' doesn't exist in table '%s'\n", field, row->table->name);
        return -1;
    }
    return put_data(row, (size_t)index, data);
}

int put_integer(row_t* row, const char* field, uint64_t data) {
    type_t type = row->table->fields[get_field_index(row->table, field)].type;
    if(type != INTEGER) {
        fprintf(stderr, "Cannot put integer in field '%s' which as type '%s'\n", field, to_string_type(type));
    }
    return put_data_from_name(row, field, (char*)&data);
}

int put_string(row_t* row, const char* field, const char* data) {
    type_t type = row->table->fields[get_field_index(row->table, field)].type;
    size_t size = row->table->fields[get_field_index(row->table, field)].size-1; // -1 for '\0'
    if(type != STRING) {
        fprintf(stderr, "Cannot put string in field '%s' which as type '%s'\n", field, to_string_type(type));
    }
    size_t data_size = strlen(data);
    if(data_size > size) {
        fprintf(stderr, "Cannot put string of size '%lu' in field of size '%lu'\n", data_size, size);
    }
    return put_data_from_name(row, field, data);
}

int get_integer(row_t* row, const char* field, uint64_t* data) {
    type_t type = row->table->fields[get_field_index(row->table, field)].type;
    if(type != INTEGER) {
        fprintf(stderr, "Cannot get integer from field '%s' which as type '%s'\n", field, to_string_type(type));
    }
    char* data_ptr = NULL;
    int res_code = get_data_from_name(row, field, &data_ptr);
    if(res_code < 0) {
        return -1;
    }
    *data = *data_ptr;
    return 0;
}

int get_string(row_t* row, const char* field, char* data, size_t data_size) {
    type_t type = row->table->fields[get_field_index(row->table, field)].type;
    size_t size = row->table->fields[get_field_index(row->table, field)].size; // -1 for '\0'
    if(type != STRING) {
        fprintf(stderr, "Cannot get string from field '%s' which as type '%s'\n", field, to_string_type(type));
    }
    char* data_ptr = NULL;
    int res_code = get_data_from_name(row, field, &data_ptr);
    strncpy(data, data_ptr, min(data_size, size));
    return res_code;
}

void dump_rows(row_t* row, size_t nb_rows) {
    table_metadata_t* table = row->table;
    static_assert(NB_TYPE == 2, "dump row should be updated for all types");
    printf("| ");
    for(size_t i = 0; i < table->nb_fields; i++) {
        printf("%*s | ", ROW_DUMP_FIELD_WIDTH, table->fields[i].name);
    }
    puts("");
    for(size_t j = 0; j < nb_rows; j++) {
        printf("| ");
        for(size_t i = 0; i < table->nb_fields; i++) {
            switch (table->fields[i].type)
            {
            case INTEGER:
                uint64_t res;
                get_integer(&row[j], table->fields[i].name, &res);
                printf("%*lu | ", ROW_DUMP_FIELD_WIDTH, res);
                break;
            case STRING:
                char buffer[ROW_DUMP_FIELD_WIDTH+1];
                get_string(&row[j], table->fields[i].name, buffer, ROW_DUMP_FIELD_WIDTH+1);
                if(buffer[ROW_DUMP_FIELD_WIDTH-1] != '\0') {
                    buffer[ROW_DUMP_FIELD_WIDTH-1] = '.';
                    buffer[ROW_DUMP_FIELD_WIDTH] = '\0';
                }
                printf("%*s | ", ROW_DUMP_FIELD_WIDTH, buffer);
            default:
                break;
            }
        }
        puts("");
    }
    
}