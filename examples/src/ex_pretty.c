// SPDX-License-Identifier: MIT
#define NT_PARSER_NCOUNT 64 // The ugly.nt document does not exceed this length.
#include "../../nt4c.h"
#include <stdlib.h>

const unsigned char input_data[] = {
#embed "../ugly.nt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
    , '\0'
};

static int pretty_print(NT_NODE *node, size_t depth) {
    if (node->data) {
        switch (node->type) {
            case NT_STR_MLN:
            case NT_SET_MLS:
            case NT_SET_DCT:
            case NT_SET_LST:
            case NT_SET_ROL:
            case NT_SET_NIL:
            case NT_STR_ROL:
            case NT_STR_COM: {
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
            case NT_TAG_COM: {
                prefix = "\x1b[33m";
                suffix = "";
                break;
            }
            case NT_TAG_LST_DCT:
            case NT_TAG_LST_LST:
            case NT_TAG_LST_MLS: {
                prefix = "\x1b[0;34m";
                break;
            }
            case NT_TAG_MLS:
            case NT_TAG_LST_ROL: {
                prefix = "\x1b[0;34m";
                suffix = "\x1b[0m";
                break;
            }
            case NT_KEY_NIL:
            case NT_KEY_MLS:
            case NT_KEY_LST:
            case NT_KEY_DCT:
            case NT_KEY_ROL: {
                prefix = "\x1b[1;34m";
                suffix = "\x1b[0m";
                break;
            }
            case NT_SET_NIL:
            case NT_SET_MLS:
            case NT_SET_DCT:
            case NT_SET_LST: {
                prefix = "\x1b[0;34m";
                break;
            }
            case NT_SET_ROL: {
                prefix = "\x1b[0;34m";
                suffix = "\x1b[0m";
                break;
            }
            case NT_STR_MLN: {
                suffix = "";
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

    return EXIT_SUCCESS;
}

int main(int, char **) {
    NT_PARSER parser = { .settings = { .blacklist = NT_SPACE|NT_NEWLINE } };

    if (nt_parse((char *) input_data, 0, &parser) > (int) parser.mem.capacity) {
        fprintf(stderr, "not enough memory for %lu nodes\n", parser.doc.length);
        return EXIT_FAILURE;
    }

    return pretty_print(parser.doc.root, 0);
}
