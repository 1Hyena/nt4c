// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

const char input_data[] = {
#embed "../multiline.nt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
    , '\0'
};

static void print_tree(NT_NODE *node, size_t depth) {
    if (!node) {
        return;
    }

    for (size_t i=0; i<depth; ++i) {
        printf("%s", "    ");
    }

    const char *prefix = "";
    const char *infix  = "???";
    const char *suffix = "\x1b[0m";

    switch (node->type) {
        case NT_NONE: {
            prefix  = "\x1b[7;37m";
            infix   = "NONE";
            break;
        }
        case NT_TOP_ROL: {
            prefix  = "\x1b[1;37m";
            infix   = "TOP_ROL";
            break;
        }
        case NT_TOP_LST: {
            prefix  = "\x1b[1;37m";
            infix   = "TOP_LST";
            break;
        }
        case NT_TOP_MLS: {
            prefix  = "\x1b[1;37m";
            infix   = "TOP_MLS";
            break;
        }
        case NT_TOP_DCT: {
            prefix  = "\x1b[1;37m";
            infix   = "TOP_DCT";
            break;
        }
        case NT_INVALID: {
            prefix  = "\x1b[7;1;31m";
            infix   = "INVALID";
            break;
        }
        case NT_DEEP: {
            prefix  = "\x1b[7;1;33m";
            infix   = "DEEP";
            break;
        }
        case NT_TAG_COM: {
            prefix  = "\x1b[33m";
            infix   = "TAG_COM";
            break;
        }
        case NT_STR_COM: {
            prefix  = "\x1b[33m";
            infix   = "STR_COM";
            break;
        }
        case NT_TAG_LST_ROL: {
            prefix  = "\x1b[0;34m";
            infix   = "TAG_LST_ROL";
            break;
        }
        case NT_TAG_LST_MLS: {
            prefix  = "\x1b[0;34m";
            infix   = "TAG_LST_MLS";
            break;
        }
        case NT_TAG_LST_LST: {
            prefix  = "\x1b[0;34m";
            infix   = "TAG_LST_LST";
            break;
        }
        case NT_TAG_LST_DCT: {
            prefix  = "\x1b[0;34m";
            infix   = "TAG_LST_DCT";
            break;
        }
        case NT_KEY_ROL: {
            prefix  = "\x1b[1;34m";
            infix   = "KEY_ROL";
            break;
        }
        case NT_KEY_MLS: {
            prefix  = "\x1b[1;34m";
            infix   = "KEY_MLS";
            break;
        }
        case NT_KEY_LST: {
            prefix  = "\x1b[1;34m";
            infix   = "KEY_LST";
            break;
        }
        case NT_KEY_DCT: {
            prefix  = "\x1b[1;34m";
            infix   = "KEY_DCT";
            break;
        }
        case NT_STR_ROL: {
            infix   = "STR_ROL";
            break;
        }
        case NT_STR_MLN: {
            infix   = "STR_MLN";
            break;
        }
        case NT_NEWLINE: {
            prefix  = "\x1b[1;30m";
            infix   = "NEWLINE";
            break;
        }
        case NT_SPACE: {
            prefix  = "\x1b[1;30m";
            infix   = "SPACE";
            break;
        }
        case NT_SET_MLS: {
            prefix  = "\x1b[0;34m";
            infix   = "SET_MLS";
            break;
        }
        case NT_SET_DCT: {
            prefix  = "\x1b[0;34m";
            infix   = "SET_DCT";
            break;
        }
        case NT_SET_LST: {
            prefix  = "\x1b[0;34m";
            infix   = "SET_LST";
            break;
        }
        case NT_SET_ROL: {
            prefix  = "\x1b[0;34m";
            infix   = "SET_ROL";
            break;
        }
        case NT_TAG_MLS: {
            prefix  = "\x1b[0;34m";
            infix   = "TAG_MLS";
            break;
        }
    }

    printf("%s%s%s", prefix, infix, suffix);

    if (node->data) {
        switch (node->type) {
            case NT_NONE:
            case NT_KEY_MLS:
            case NT_KEY_DCT:
            case NT_KEY_LST:
            case NT_KEY_ROL: {
                printf(" (%.*s)", (int) node->size, node->data);
                break;
            }
            case NT_SPACE: {
                printf(" (%lu)", node->size);
                break;
            }
            default: break;
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

size_t i = node_count;

    //for (size_t i=0; i< 146; ++i) {
        nt_parser_set_memory(&parser, nodes, i /*node_count*/);

        //nt_parser_set_blacklist(&parser, NT_SPACE|NT_NEWLINE);

        int result = nt_parse(input_data, 0, &parser);

        if (result > (int) i /*node_count*/) {
            fprintf(
                stderr, "[%3lu] insufficient memory for %lu nodes (result was %d)\n", i, parser.node.count, result
            );

            //print_tree(parser.nest.root, 0);
            //break;
            //return EXIT_FAILURE;
        }
        else {
            print_tree(parser.nest.root, 0);

            fprintf(
                stderr, "[%3lu] sufficient memory for %lu nodes (result was %d)\n", i, parser.node.count, result
            );

            //break;
        }
    //}



    return EXIT_SUCCESS;
}
