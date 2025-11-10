// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

const char input_data[] = {
#embed "../repository.nt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
    , '\0'
};

static void pretty_print(NT_NODE *node, size_t depth) {
    if (node->data) {
        switch (node->type) {
            case NT_OP_SET:
            case NT_OP_SET_ROL:
            case NT_STRING:
            case NT_COMMENT: {
                break;
            }
            default: {
                for (size_t i=0; i<depth; ++i) {
                    printf("%s", "    ");
                }

                break;
            }
        }

        const char *prefix = "";
        const char *suffix = "\x1b[0m\n";

        switch (node->type) {
            case NT_NONE: {
                prefix = "\x1b[7;37m";
                break;
            }
            case NT_INVALID: {
                prefix = "\x1b[7;31m";
                break;
            }
            case NT_COMMENT_TAG: {
                prefix = "\x1b[33m";
                suffix = "";
                break;
            }
            case NT_LIST_TAG: {
                prefix = "\x1b[0;34m";
                suffix = "\x1b[0m";
                break;
            }
            case NT_LIST_KEY:
            case NT_DICT_KEY:
            case NT_KEY: {
                prefix = "\x1b[1;34m";
                suffix = "\x1b[0m";
                break;
            }
            case NT_OP_SET: {
                prefix = "\x1b[0;34m";
                break;
            }
            case NT_OP_SET_ROL: {
                prefix = "\x1b[0;34m";
                suffix = "\x1b[0m";
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
    constexpr size_t node_count = 128;
    NT_NODE nodes[node_count];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, node_count);
    nt_parser_set_blacklist(&parser, NT_SPACE|NT_NEWLINE);

    if (nt_parse(input_data, 0, &parser) > (int) node_count) {
        fprintf(
            stderr, "insufficient memory for %lu nodes\n", parser.node.count
        );
    }
    else pretty_print(parser.node.root, 0);

    return EXIT_SUCCESS;
}
