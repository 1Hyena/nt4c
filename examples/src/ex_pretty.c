// SPDX-License-Identifier: MIT
#define NT_PARSER_NCOUNT 64 // The ugly.nt document does not exceed this length.
#include "../../nt4c.h"
#include <stdlib.h>

const unsigned char input_data[] = {
#embed "../ugly.nt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
    , '\0'
};

static int pretty_print(NT_NODE *node, size_t depth) {
    constexpr NT_TYPE body_nodes = (
        NT_STR_MLN | NT_SET_MLS | NT_SET_DCT | NT_SET_LST | NT_SET_ROL |
        NT_SET_NIL | NT_STR_ROL | NT_STR_COM
    );

    constexpr NT_TYPE navy_nodes = (
        NT_TAG_LST_ROL  | NT_SET_MLS |
        NT_TAG_LST_DCT  | NT_SET_NIL |
        NT_TAG_LST_LST  | NT_SET_DCT |
        NT_TAG_LST_MLS  | NT_SET_LST |
        NT_TAG_MLS      | NT_SET_ROL
    );

    constexpr NT_TYPE blue_nodes = (
        NT_KEY_NIL | NT_KEY_MLS | NT_KEY_LST | NT_KEY_DCT | NT_KEY_ROL
    );

    constexpr NT_TYPE olive_nodes = NT_TAG_COM | NT_STR_COM;

    constexpr NT_TYPE no_newline = (
        NT_TAG_COM | NT_TAG_MLS | NT_TAG_LST_ROL | NT_KEY_NIL | NT_KEY_MLS |
        NT_KEY_LST | NT_KEY_DCT | NT_KEY_ROL     | NT_SET_ROL | NT_STR_MLN
    );

    if (node->data) {
        if ((node->type & body_nodes) == false) {
            for (size_t i=0; i<depth; ++i) {
                printf("%s", "    ");
            }
        }

        const char *prefix = (
            (node->type & navy_nodes    ) ? "\x1b[0;34m" :
            (node->type & blue_nodes    ) ? "\x1b[1;34m" :
            (node->type & olive_nodes   ) ? "\x1b[0;33m" :
            (node->type & NT_NONE       ) ? "\x1b[7;37m" :
            (node->type & NT_INVALID    ) ? "\x1b[7;31m" : ""
        );

        const char *suffix = node->type & no_newline ? "\x1b[0m" : "\x1b[0m\n";

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
