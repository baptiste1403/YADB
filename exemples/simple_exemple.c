#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "database.h"
#include "row.h"
#include "logger.h"

int main(int argc, char **argv) {

    (void)argc;
    (void)argv;

    database_t database = {0};

    table_metadata_t* table = create_table(&database, "person");

    add_field(table, "id", INTEGER, 8);
    add_field(table, "age", INTEGER, 8);
    add_field(table, "name", STRING, 50);

    row_list_t row_list = {0};
    row_list.table = get_table(&database, "person");
    add_row(&row_list);
    add_row(&row_list);

    log_message(LOG_INFO, "create and dump items :\n");

    // this is bad, you must verify return type !!!
    put_string(&row_list, 0, "name", "Jean BON");
    put_string(&row_list, 1, "name", "Guy LIGUILY");
    put_integer(&row_list, 0, "age", 35);
    put_integer(&row_list, 1, "age", 47);
    put_integer(&row_list, 0, "id", 0);
    put_integer(&row_list, 1, "id", 1);

    //dump_rows(person_row, 2);

    insert_rows(&row_list);

    free_row_list(&row_list);

    load_rows(&row_list, get_table(&database, "person"));

    dump_rows(&row_list);

    log_message(LOG_INFO, "save/load and dump database :\n");

    save_database(&database);

    free_database(&database);

    load_database(&database);

    dump_table_metadata(&database.items[0]);

    free_database(&database);

    return EXIT_SUCCESS;
}