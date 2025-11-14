// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

const unsigned char input_data[] = {
#embed "../repository.nt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
    , '\0'
};

static void print_tree(NT_NODE *node, size_t depth) {
    if (!node) {
        return;
    }

    for (size_t i=0; i<depth; ++i) {
        printf("%s", "    ");
    }

    constexpr NT_TYPE navy_nodes = (
        NT_TAG_LST_ROL  | NT_SET_MLS |
        NT_TAG_LST_DCT  | NT_SET_NIL |
        NT_TAG_LST_LST  | NT_SET_DCT |
        NT_TAG_LST_MLS  | NT_SET_LST |
        NT_TAG_LST_NIL  | NT_SET_ROL |
        NT_TAG_MLS
    );

    constexpr NT_TYPE blue_nodes = (
        NT_KEY_NIL | NT_KEY_MLS | NT_KEY_LST | NT_KEY_DCT | NT_KEY_ROL
    );

    constexpr NT_TYPE green_nodes = NT_STR_ROL | NT_STR_MLN;
    constexpr NT_TYPE gray_nodes  = NT_SPACE   | NT_NEWLINE;
    constexpr NT_TYPE olive_nodes = NT_TAG_COM | NT_STR_COM;

    const char *prefix = (
        (node->type & navy_nodes    ) ? "\x1b[0;34m" :
        (node->type & blue_nodes    ) ? "\x1b[1;34m" :
        (node->type & olive_nodes   ) ? "\x1b[0;33m" :
        (node->type & green_nodes   ) ? "\x1b[0;32m" :
        (node->type & gray_nodes    ) ? "\x1b[1;30m" :
        (node->type & NT_NONE       ) ? "\x1b[7;37m" :
        (node->type & NT_INVALID    ) ? "\x1b[7;31m" : ""
    );

    const char *suffix = "\x1b[0m";

    printf("%s%s%s", prefix, nt_code(node->type), suffix);

    if (node->data) {
        if (node->type == NT_SPACE) {
            printf("\x1b[0;1;30;3m, length %lu\x1b[0m", node->size);
        }
        else {
            size_t line_size;
            nt4c_str_line_size(node->data, node->size, &line_size);

            if (line_size) {
                printf("%s", "\x1b[1;30m:\x1b[0;3m ");

                if (line_size < 10) {
                    printf("%.*s\x1b[0m", (int) line_size, node->data);
                }
                else {
                    printf("%.*s%s\x1b[0m", 10, node->data, "\x1b[1;30m...");
                }
            }
        }
    }

    printf("%s", "\n");
    ++depth;

    for (NT_NODE *child = node->children; child; child = child->next) {
        print_tree(child, depth);
    }
}

int main(int, char **) {
    constexpr size_t node_count = 200;
    NT_NODE nodes[node_count];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, node_count);

    int result = nt_parse((char *) input_data, sizeof(input_data), &parser);

    if (result > (int) node_count) {
        fprintf(stderr, "not enough memory for %lu nodes\n", parser.doc.length);

        return EXIT_FAILURE;
    }

    print_tree(parser.doc.root, 0);

    return EXIT_SUCCESS;
}
