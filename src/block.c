#include "block.h"

/**
 * Insert a tuple in the block if there is enought remaining space
 * @param block the block where to insert the tuple
 * @param tuple the data (tuple) to be inserted
 * @param tuple_size the size of the data (tuple) to be inserted in bytes
 * @return zero on success, non-zero on error 
*/
int block_put_tuple(block_t* block, const char* tuple, size_t tuple_size) {

    if(tuple_size + sizeof(tuple_header_t) >= BLOCK_BODY-sizeof(tuple_ptr_t)) {
        log_message(LOG_ERROR, "cannot put tuple bigger than %lu bytes", BLOCK_BODY-sizeof(tuple_ptr_t));
        return -1;
    }

    tuple_header_t header = {0};
    header.size = tuple_size;
    // calculate the new position of the end of rows
    tuple_ptr_t nrow_end = block->header.row_end - tuple_size - sizeof(tuple_header_t);

    if(nrow_end < block->header.row_ptr_end) {
        log_message(LOG_ERROR, "the block is full");
        return -1;
    }

    /* copy the tuple at the new row end ptr address in the block
        (new ptr to end)    (old ptr to end = new ptr + tuple_size)
               \/                  \/
                |     the tuple     | already existing tuple....
    */ 
    memcpy(&block->data[block_ptr_to_data_index(nrow_end)], &header, sizeof(tuple_header_t));
    memcpy(&block->data[block_ptr_to_data_index(nrow_end + sizeof(tuple_header_t))], tuple, tuple_size);
    block->header.row_end = nrow_end;

    /*write the index of the row at the end of the row's ptrs*/
    memcpy(&block->data[block_ptr_to_data_index(block->header.row_ptr_end)], &nrow_end, sizeof(tuple_ptr_t));
    block->header.row_ptr_end += sizeof(tuple_ptr_t);
    // increment number of rows
    block->header.nb_rows++;
    return 0;
}


int block_get_tuple_view(block_t* block, size_t index, tuple_view_t* res) {
    if(block->header.nb_rows == 0) {
        log_message(LOG_ERROR, "block is empty");
        return -1;
    }
    if(index >= block->header.nb_rows) {
        log_message(LOG_ERROR, "index out of range : %lu", index);
        return -1;
    }

    // address of first tuple pointer <=> array in c
    tuple_ptr_t* tuple_ptrs = (tuple_ptr_t*)&block->data;

    tuple_ptr_t tuple_ptr = tuple_ptrs[index];

    res->header = *((tuple_header_t*)&block->data[block_ptr_to_data_index(tuple_ptr)]);
    res->data = &block->data[block_ptr_to_data_index(tuple_ptr + sizeof(tuple_header_t))];

    return 0;
}

/**
 * @param block the block where to perform the operation
 * @return the number of bytes of data that can be inserted in the block (doesn't take into account hole in removed data)
*/
size_t block_get_remain(const block_t* block) {
    int diff = (block->header.row_end - sizeof(tuple_header_t)) - (block->header.row_ptr_end + sizeof(tuple_ptr_t));
    return diff > 0 ? diff : 0;
}

void block_reset(block_t* block) {
    block->header.nb_rows=0;
    block->header.row_ptr_end=sizeof(block_header_t);
    block->header.row_end=BLOCK_SIZE;
}