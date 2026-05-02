// SPDX-License-Identifier: MIT
#include "utils.h"
#include "../../nt4c.h"


static int callback(NT_TYPE, const char *, size_t, void *, size_t);

typedef struct QUERY {
    const char *key;

    struct {
        size_t depth;
        NT_TYPE type;
    } result;
} QUERY;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage:  %s %s %s\n", argv[0], "key", "FILE");
        return EXIT_FAILURE;
    }

    QUERY query = {};

    query.key = argv[1];

    size_t input_size;
    char *input_data = read_file_to_memory(argv[2], &input_size);

    if (!input_data) {
        fprintf(stderr, "%s\n", "failed to read the input file");
        return EXIT_FAILURE;
    }

    NT_PARSER parser = nt_make_parser();

    nt_parser_set_callback(&parser, callback);
    nt_parser_set_userdata(&parser, &query);
    nt_parser_set_blacklist(&parser, NT_INTERSPACE);
    nt_parser_parse(&parser, input_data, input_size);

    return free_and_return(input_data, EXIT_SUCCESS);
}

static int callback(
    NT_TYPE node_type, const char *data, size_t data_size,
    void *userdata, size_t depth
) {
    QUERY *query = (QUERY *) userdata;

    NT_TYPE group_type = nt_type_type(node_type); // Let's generalize the type.

    if (group_type == NT_KEY
    && !query->result.depth
    && !strncmp(data, query->key, data_size)
    && strlen(query->key) == data_size) {
        query->result.depth = depth;

        return 0;
    }

    if (depth <= query->result.depth) {
        return -1;
    }

    if (!query->result.depth) {
        return 0;
    }

    if (query->result.type == NT_NONE) {
        if ((group_type & (NT_STR|NT_KEY|NT_LST)) == false) {
            if (node_type == NT_COMMENT) {
                for (size_t i = query->result.depth + 1; i < depth; ++i) {
                    printf("%s", "    ");
                }

                printf("#%.*s\n", (int) data_size, data);
            }

            return 0;
        }

        query->result.type = group_type;
    }

    if (query->result.type == NT_STR) {
        if (group_type == NT_STR && depth == query->result.depth + 1) {
            printf("%.*s\n", (int) data_size, data);
        }

        return 0;
    }

    if (node_type == NT_INDENT) {
        for (size_t i = query->result.depth + 1; i < depth; ++i) {
            printf("%s", "    ");
        }
    }
    else printf("%.*s", (int) data_size, data);

    return 0;
}
