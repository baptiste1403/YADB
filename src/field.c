#include "field.h"

const char* to_string_type(type_t type) {
    static_assert(NB_TYPE == 2, "string representation should be updated for all types");
    static const char* integer_rep = "INTEGER";
    static const char* string_rep = "STRING";
    switch (type) {
        case INTEGER:
            return integer_rep;
        case STRING:
            return string_rep;
        default:
            assert(0 && "Unreachable");
            break;
    }
    return NULL;
}