// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

const char input_data[] = {
#embed "../repository.nt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
    , '\0'
};

static void pretty_print(NT_NODE *node, size_t depth) {
    if (node->data) {
        if (node->type != NT_STRING) {
            for (size_t i=0; i<depth; ++i) {
                printf("%s", "    ");
            }
        }

        const char *prefix = "";
        const char *suffix = "\n";

        switch (node->type) {
            case NT_INVALID: {
                prefix = "\x1b[7;31m";
                suffix = "\x1b[0m\n";
                break;
            }
            case NT_COMMENT: {
                prefix = "\x1b[33m#";
                suffix = "\x1b[0m\n";
                break;
            }
            case NT_KEY: {
                prefix = "\x1b[1;34m";
                suffix = "\x1b[0m:\n";

                if (node->children && node->children->type == NT_STRING) {
                    suffix = "\x1b[0m: ";
                }

                break;
            }
            default: break;
        }

        printf("%s%.*s%s", prefix, (int) node->size, node->data, suffix);
        ++depth;
    }

    for (NT_NODE *child = node->children; child; child = child->next) {
        pretty_print(child, depth);
    }
}

int main(int, char **) {
    constexpr size_t node_count = 32;
    NT_NODE nodes[node_count];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, node_count);
    nt_parse(input_data, 0, &parser);
    pretty_print(parser.node.root, 0);

    return EXIT_SUCCESS;
}
