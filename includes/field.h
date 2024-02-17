#ifndef __FIELD__
#define __FIELD__

#include <stdlib.h>
#include <assert.h>

#define FIELD_NAME_SIZE 30

typedef enum {
    INTEGER,
    STRING,
    NB_TYPE
} type_t;

typedef struct {
    char name[FIELD_NAME_SIZE];
    type_t type;
    size_t size;
    size_t offset;
} field_metadata_t;

const char* to_string_type(type_t type);

#endif // __FIELD__