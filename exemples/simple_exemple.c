#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "database.h"
#include "row.h"


// --------- MAIN ---------

int main(int argc, char **argv) {

    database_t database = {0};
    database.nb_table = 1;
    database.tables = (table_metadata_t*)malloc(sizeof(table_metadata_t));

    table_builder_t builder = {0};
    init_table(&builder, "Person", 3);
    if(set_field(&builder, "id", INTEGER, 8) < 0) return EXIT_FAILURE;
    if(set_field(&builder, "age", INTEGER, 8) < 0) return EXIT_FAILURE;
    if(set_field(&builder, "name", STRING, 50) < 0) return EXIT_FAILURE;
    get_table(&builder, &database.tables[0]);

    puts("\ncreate and dump rows :\n");

    row_t person_row[2];
    person_row[0] = create_table_row(&database.tables[0]);
    person_row[1] = create_table_row(&database.tables[0]);

    // this is bad, you must verify return type !!!
    put_string(&person_row[0], "name", "Jean BON");
    put_string(&person_row[1], "name", "Guy LIGUILY");
    put_integer(&person_row[0], "age", 35);
    put_integer(&person_row[1], "age", 47);
    put_integer(&person_row[0], "id", 0);
    put_integer(&person_row[1], "id", 1);

    dump_rows(person_row, 2);

    puts("\nsave/load and dump database :\n");

    save_database(&database);

    database = (database_t){0};

    load_database(&database);

    dump_table_metadata(&database.tables[0]);

    return EXIT_SUCCESS;
}