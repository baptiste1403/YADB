#include "row.h"

#define ROW_DUMP_FIELD_WIDTH 20

row_t _create_row(table_metadata_t* table, const char* file, size_t line) {
    row_t res = {
        .data = calloc(table->row_size, 1)
    };

    if(res.data == NULL) {
        _log_message(LOG_ERROR, file, line, "malloc error : %s", strerror(errno));
        exit(-1);
    }
    return res;
}

void free_row(row_t* row) {
    if(row->data != NULL) {
        free(row->data);
    }
}

void free_row_list(row_list_t* row_list) {
    if(row_list->count > 0) {
        for(size_t i = 0; i < row_list->count; i++) {
            free_row(&row_list->items[i]);
        }
        *row_list = (row_list_t){0};
    }
}

int _get_data_ptr(row_list_t* row_list, size_t index, size_t field, char** data, const char* file, size_t line) {
    if(field >= row_list->table->count) {
        _log_message(LOG_ERROR, file, line,  "field index out of range for table '%s' with index %lu\n", row_list->table->name, field);
        return -1;
    }
    if(index >= row_list->count) {
        _log_message(LOG_ERROR, file, line, "row index out of range");
        return -1;
    }
    field_metadata_t field_metadata = row_list->table->items[field];
    *data = row_list->items[index].data+field_metadata.offset;
    return 0;
}

int _get_data_ptr_from_name(row_list_t* row_list, size_t index, const char* field, char** data, const char* file, size_t line) {
    int field_index = get_field_index(row_list->table, field);
    if(field_index < 0) {
        _log_message(LOG_ERROR, file, line, "field '%s' doesn't exist in table '%s'\n", field, row_list->table->name);
        return -1;
    }
    if(index >= row_list->count) {
        _log_message(LOG_ERROR, file, line, "row index out of range");
        return -1;
    }
    return _get_data_ptr(row_list, (size_t)index, field_index, data, file, line);
}

int _put_data(row_list_t* row_list, size_t index, size_t field, const char* data, size_t data_size, const char* file, size_t line) {
    if(field >= row_list->table->count) {
        _log_message(LOG_ERROR, file, line, "field index out of range for table '%s' with index %lu\n", row_list->table->name, field);
        return -1;
    }
    if(index >= row_list->count) {
        _log_message(LOG_ERROR, file, line, "row index out of range");
        return -1;
    }
    field_metadata_t field_metadata = row_list->table->items[field];
    char* dest_ptr = row_list->items[index].data+field_metadata.offset;
    if(data_size > field_metadata.size) {
        _log_message(LOG_ERROR, file, line, "Cannot put data of size %d in field '%s' of size %d", data_size, field_metadata.name, field_metadata.size);
        return -1;
    }
    memcpy(dest_ptr, data, data_size);
    return 0;
}

int _put_data_from_name(row_list_t* row_list, size_t index, const char* field, const char* data, size_t data_size, const char* file, size_t line) {
    int field_index = get_field_index(row_list->table, field);
    if(field_index < 0) {
        _log_message(LOG_ERROR, file, line, "field '%s' doesn't exist in table '%s'\n", field, row_list->table->name);
        return -1;
    }
    if(index >= row_list->count) {
        _log_message(LOG_ERROR, file, line, "row index out of range");
        return -1;
    }
    return _put_data(row_list, index, (size_t)field_index, data, data_size, file, line);
}

int _put_integer(row_list_t* row_list, size_t index, const char* field, uint64_t data, const char* file, size_t line) {
    type_t type = row_list->table->items[get_field_index(row_list->table, field)].type;
    if(type != INTEGER) {
        _log_message(LOG_ERROR, file, line, "Cannot put integer in field '%s' which as type '%s'\n", field, to_string_type(type));
    }
    if(index >= row_list->count) {
        _log_message(LOG_ERROR, file, line, "row index out of range");
        return -1;
    }
    return _put_data_from_name(row_list, index, field, (char*)&data, sizeof(uint64_t), file, line);
}

int _put_string(row_list_t* row_list, size_t index, const char* field, const char* data, const char* file, size_t line) {
    type_t type = row_list->table->items[get_field_index(row_list->table, field)].type;
    size_t size = row_list->table->items[get_field_index(row_list->table, field)].size-1; // -1 for '\0'
    if(index >= row_list->count) {
        _log_message(LOG_ERROR, file, line, "row index out of range");
        return -1;
    }
    if(type != STRING) {
        _log_message(LOG_ERROR, file, line, "Cannot put string in field '%s' which as type '%s'\n", field, to_string_type(type));
    }
    size_t data_size = strlen(data);
    if(data_size > size) {
        _log_message(LOG_ERROR, file, line, "Cannot put string of size '%lu' in field of size '%lu'\n", data_size, size);
    }
    return _put_data_from_name(row_list, index, field, data, data_size+1, file, line); // add the '\0'
}

int _get_integer(row_list_t* row_list, size_t index, const char* field, uint64_t* data, const char* file, size_t line) {
    type_t type = row_list->table->items[get_field_index(row_list->table, field)].type;
    if(index >= row_list->count) {
        _log_message(LOG_ERROR, file, line, "row index out of range");
        return -1;
    }
    if(type != INTEGER) {
        _log_message(LOG_ERROR, file, line, "Cannot get integer from field '%s' which as type '%s'\n", field, to_string_type(type));
    }
    char* data_ptr = NULL;
    int res_code = _get_data_ptr_from_name(row_list, index, field, &data_ptr, file, line);
    if(res_code < 0) {
        return -1;
    }
    memcpy(data, data_ptr, sizeof(uint64_t));
    return 0;
}

int _get_string(row_list_t* row_list, size_t index, const char* field, char* data, size_t data_size, const char* file, size_t line) {
    type_t type = row_list->table->items[get_field_index(row_list->table, field)].type;
    size_t size = row_list->table->items[get_field_index(row_list->table, field)].size; // -1 for '\0'
    if(index >= row_list->count) {
        _log_message(LOG_ERROR, file, line, "row index out of range");
        return -1;
    }
    if(type != STRING) {
        _log_message(LOG_ERROR, file, line,  "Cannot get string from field '%s' which as type '%s'\n", field, to_string_type(type));
    }
    char* data_ptr = NULL;
    int res_code = _get_data_ptr_from_name(row_list, index, field, &data_ptr, file, line);
    if(data_size < size) {
        _log_message(LOG_WARN, file, line, "Destination buffer is smaller than the field, it may lead to truncated data");
    }
    strncpy(data, data_ptr, data_size);
    return res_code;
}

int _load_rows(row_list_t* row_list, table_metadata_t* table, const char* file, size_t line) {
    int defer_return_value = 0;
    FILE* fd = NULL;
    char name_buffer[TABLE_NAME_SIZE + 5];

    strcpy(name_buffer, table->name);
    strcat(name_buffer, ".yadb");

    row_list->table = table;

    fd = fopen(name_buffer, "r");

    size_t row_count;
    if(fread(&row_count, sizeof(row_count), 1, fd) != 1) {
        _log_message(LOG_ERROR, file, line, "Cannot read in file\n");
        return_defer(-1);
    }

    for(size_t i = 0; i < row_count; i++) {
        row_t row = _create_row(row_list->table, file, line);
        if(fread(row.data, row_list->table->row_size, 1, fd) != 1) {
            _log_message(LOG_ERROR, file, line, "Cannot read in file\n");
            return_defer(-1);
        }
        list_append(row_list, row);
    }

    defer:
        if(defer_return_value == -1) {
            free_row_list(row_list);
        }
        fclose(fd);
        return defer_return_value;
}

int _insert_rows(row_list_t* row_list, const char* file, size_t line) {
    int defer_return_value = 0;
    FILE* fd = NULL;
    char name_buffer[TABLE_NAME_SIZE + 5];

    strcpy(name_buffer, row_list->table->name);
    strcat(name_buffer, ".yadb");
    
    if(access(name_buffer, F_OK) == 0) {
        fd = fopen(name_buffer, "r+b");
        size_t nb_row_in_file;
        if(fread(&nb_row_in_file, sizeof(nb_row_in_file), 1, fd) != 1) {
            _log_message(LOG_ERROR, file, line, "Cannot read in file\n");
            return_defer(-1);
        }
        if(fseek(fd, 0, SEEK_SET) < 0) {
            _log_message(LOG_ERROR, file, line, "Cannot fseek in file\n");
            return_defer(-1);
        }
        nb_row_in_file += row_list->count;
        if(fwrite(&nb_row_in_file, sizeof(nb_row_in_file), 1, fd) != 1) {
            _log_message(LOG_ERROR, file, line, "Cannot write in file\n");
            return_defer(-1);
        }
        if(fseek(fd, 0, SEEK_END) < 0) {
            _log_message(LOG_ERROR, file, line, "Cannot fseek in file\n");
            return_defer(-1);
        }
    } else {
        fd = fopen(name_buffer, "w");
        if(fwrite(&row_list->count, sizeof(row_list->count), 1, fd) != 1) {
            _log_message(LOG_ERROR, file, line, "Cannot write in file\n");
            return_defer(-1);
        }
    }

    for(size_t i = 0; i < row_list->count; i++) {
        if(fwrite(row_list->items[i].data, row_list->table->row_size, 1, fd) != 1) {
            _log_message(LOG_ERROR, file, line, "Cannot write in file\n");
            return_defer(-1);
        }
    }

    defer:
        fclose(fd);
        return defer_return_value;
}

int _add_row(row_list_t* row_list, const char* file, size_t line) {
    if(row_list->table == NULL) {
        _log_message(LOG_ERROR, file, line, "uninitialized row list");
        return -1;
    }
    list_append(row_list, _create_row(row_list->table, file, line));
    return 0;
}

void dump_rows(row_list_t* row_list) {
    table_metadata_t* table = row_list->table;
    static_assert(NB_TYPE == 2, "dump row should be updated for all types");
    printf("| ");
    for(size_t i = 0; i < table->count; i++) {
        printf("%*s | ", ROW_DUMP_FIELD_WIDTH, table->items[i].name);
    }
    puts("");
    for(size_t j = 0; j < row_list->count; j++) {
        printf("| ");
        uint64_t res;
        char buffer[256];
        for(size_t i = 0; i < table->count; i++) {
            switch (table->items[i].type)
            {
            case INTEGER:
                get_integer(row_list, j, table->items[i].name, &res);
                printf("%*lu | ", ROW_DUMP_FIELD_WIDTH, res);
                break;
            case STRING:
                get_string(row_list, j, table->items[i].name, buffer, 256);
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