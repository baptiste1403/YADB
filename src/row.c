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
    size_t size = row_list->table->items[get_field_index(row_list->table, field)].size;
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
    return _put_data_from_name(row_list, index, field, data, data_size, file, line);
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

int _get_string(row_list_t* row_list, size_t index, const char* field, char* buffer, size_t buffer_size, const char* file, size_t line) {
    type_t type = row_list->table->items[get_field_index(row_list->table, field)].type;
    size_t size = row_list->table->items[get_field_index(row_list->table, field)].size;
    if(index >= row_list->count) {
        _log_message(LOG_ERROR, file, line, "row index out of range");
        return -1;
    }
    if(type != STRING) {
        _log_message(LOG_ERROR, file, line,  "Cannot get string from field '%s' which as type '%s'\n", field, to_string_type(type));
    }
    char* data_ptr = NULL;
    int res_code = _get_data_ptr_from_name(row_list, index, field, &data_ptr, file, line);
    if(buffer_size < (size + 1)) {
        _log_message(LOG_WARN, file, line, "Destination buffer is smaller than the field, it may lead to truncated data");
    }
    strncpy(buffer, data_ptr, buffer_size-1); // for \0 ???
    return res_code;
}

int _load_rows_block(row_list_t* row_list, table_metadata_t* table, const char* file, size_t line) {
    int defer_return_value = 0;
    FILE* fd = NULL;
    char name_buffer[TABLE_NAME_SIZE + 5];

    strcpy(name_buffer, table->name);
    strcat(name_buffer, ".yadb");

    row_list->table = table;
    fd = fopen(name_buffer, "r");

    uint32_t block_count;
    if(fread(&block_count, sizeof(uint32_t), 1, fd) != 1) {
        _log_message(LOG_ERROR, file, line, "Cannot read in file\n");
        return_defer(-1);
    }

    block_t block_buffer = {0};

    for(size_t i = 0; i < block_count; i++) {
        if(fread(&block_buffer, sizeof(block_t), 1, fd) != 1) {
            _log_message(LOG_ERROR, file, line, "Cannot read in file\n");
            return_defer(-1);
        }

        for(size_t j = 0; j < block_buffer.header.nb_rows; j++) {
            row_t row = _create_row(row_list->table, file, line);
            tuple_view_t tuple;
            block_get_tuple_view(&block_buffer, j, &tuple);
            memcpy(row.data, tuple.data, tuple.header.size);
            list_append(row_list, row);
        }
    }

    defer:
        fclose(fd);
        return defer_return_value;
}

int _insert_rows_block(row_list_t* row_list, const char* file, size_t line) {
    int defer_return_value = 0;
    FILE* fd = NULL;
    char name_buffer[TABLE_NAME_SIZE + 5];

    strcpy(name_buffer, row_list->table->name);
    strcat(name_buffer, ".yadb");

    block_t block_buffer = {0};
    uint32_t nb_blocks = 0;

    block_reset(&block_buffer);

    if(access(name_buffer, F_OK) == 0) {
        fd = fopen(name_buffer, "r+b");
        if(fread(&nb_blocks, sizeof(uint32_t), 1, fd) != 1) {
            _log_message(LOG_ERROR, file, line, "Cannot read from file\n");
            return_defer(-1);
        }
        if(nb_blocks > 0) {
            if(fseek(fd, sizeof(uint32_t) +(nb_blocks - 1)*BLOCK_SIZE, SEEK_SET) < 0) {
                _log_message(LOG_ERROR, file, line, "Cannot fseek in file\n");
                return_defer(-1);
            }
            if(fread(&block_buffer, sizeof(block_t), 1, fd) != 1) {
                _log_message(LOG_ERROR, file, line, "Cannot read from file\n");
                return_defer(-1);
            }
        } else {
            fd = fopen(name_buffer, "w");
            if(fwrite(&nb_blocks, sizeof(uint32_t), 1, fd) != 1) {
                _log_message(LOG_ERROR, file, line, "Cannot write in file\n");
                return_defer(-1);
            }
            nb_blocks = 1;
        }
    } else {
        fd = fopen(name_buffer, "w");
        if(fwrite(&nb_blocks, sizeof(uint32_t), 1, fd) != 1) {
            _log_message(LOG_ERROR, file, line, "Cannot write in file\n");
            return_defer(-1);
        }
        nb_blocks = 1;
    }

    for(size_t i = 0; i < row_list->count; i++) {
        if(block_get_remain(&block_buffer) < row_list->table->row_size) {
            if(fseek(fd, sizeof(uint32_t) + (nb_blocks-1)*BLOCK_SIZE, SEEK_SET) < 0) {
                _log_message(LOG_ERROR, file, line, "Cannot fseek in file\n");
                return_defer(-1);
            }
            if(fwrite(&block_buffer, sizeof(block_t), 1, fd) != 1) {
                _log_message(LOG_ERROR, file, line, "Cannot write in file\n");
                return_defer(-1);
            }
            nb_blocks++;
            block_reset(&block_buffer);
        }

        if(block_put_tuple(&block_buffer, row_list->items[i].data, row_list->table->row_size) != 0) {
            _log_message(LOG_ERROR, file, line, "Cannot add tuple to block\n");
            return_defer(-1);
        }
    }

    if(fseek(fd, sizeof(uint32_t) + (nb_blocks-1)*BLOCK_SIZE, SEEK_SET) < 0) {
        _log_message(LOG_ERROR, file, line, "Cannot fseek in file\n");
        return_defer(-1);
    }
    if(fwrite(&block_buffer, sizeof(block_t), 1, fd) != 1) {
        _log_message(LOG_ERROR, file, line, "Cannot write in file\n");
        return_defer(-1);
    }

    if(fseek(fd, 0, SEEK_SET) < 0) {
        _log_message(LOG_ERROR, file, line, "Cannot fseek in file\n");
        return_defer(-1);
    }
    if(fwrite(&nb_blocks, sizeof(uint32_t), 1, fd) != 1) {
        _log_message(LOG_ERROR, file, line, "Cannot write in file\n");
        return_defer(-1);
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