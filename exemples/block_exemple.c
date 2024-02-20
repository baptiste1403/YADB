#include <stdio.h>
#include "logger.h"
#include "block.h"

int main(void) {

    static_assert(sizeof(block_t) == BLOCK_SIZE);
    log_message(LOG_INFO, "size of struct block_t : %lu", sizeof(block_t));
    log_message(LOG_INFO, "size of struct block_header_t : %lu", sizeof(block_header_t));
    log_message(LOG_INFO, "size of block body : %lu\n", BLOCK_BODY);

    block_t* block = (block_t*)calloc(1, sizeof(block_t));

    block->header.nb_rows=0;
    block->header.row_ptr_end=sizeof(block_header_t);
    block->header.row_end=BLOCK_SIZE;

    log_message(LOG_INFO, "nb_rows : %u", block->header.nb_rows);
    log_message(LOG_INFO, "row_ptr_end : %u", block->header.row_ptr_end);
    log_message(LOG_INFO, "row_end : %u\n", block->header.row_end);

    char* msg = "😜"; // yes it support utf-8 !!!
    while(block_put_tuple(block, msg, strlen(msg)) == 0) {}
    log_message(LOG_INFO, "remain : %lu\n", block_get_remain(block));

    log_message(LOG_INFO, "size of msg : %lu\n", strlen(msg)+1);

    log_message(LOG_INFO, "nb_rows : %u", block->header.nb_rows);
    log_message(LOG_INFO, "row_ptr_end : %u", block->header.row_ptr_end);
    log_message(LOG_INFO, "row_end : %u\n", block->header.row_end);

    FILE* file = fopen("test.block", "wb");
    if(file == NULL) {
        log_message(LOG_ERROR, "cannot open file");
        return EXIT_FAILURE;
    }

    if(fwrite(block, sizeof(block_t), 1, file) != 1) {
        fclose(file);
        log_message(LOG_ERROR, "cannot write in file");
        return EXIT_FAILURE;
    }

    fclose(file);

    file = fopen("test.block", "rb");
    if(file == NULL) {
        log_message(LOG_ERROR, "cannot open file");
        return EXIT_FAILURE;
    }

    *block = (block_t){0};

    if(fread(block, sizeof(block_t), 1, file) != 1) {
        fclose(file);
        log_message(LOG_ERROR, "cannot read file");
        return EXIT_FAILURE;
    }

    char buf[256];
    tuple_view_t tuple;
    block_get_tuple_view(block, 0, &tuple);
    strncpy(buf, tuple.data, tuple.header.size); // because data as texte isn't null terminated
    log_message(LOG_INFO, "data in block : %s of size %lu", buf, tuple.header.size);

    fclose(file);
    free(block);

    return EXIT_SUCCESS;
}