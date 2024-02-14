#include "field.h"

char* to_string_type(type_t type) {
    static_assert(NB_TYPE == 2, "string representation should be updated for all types");
    switch (type) {
        case INTEGER:
            return "INTEGER";
        case STRING:
            return "STRING";
        default:
            assert("Unreachable");
            break;
    }
    return NULL;
}