// SPDX-License-Identifier: MIT
#include "utils.h"
#define NT_PARSER_NCOUNT 64 // The ugly.nt document does not exceed this length.
#include "../../nt4c.h"


static int pretty_print(NT_NODE *node, size_t depth) {
    constexpr NT_TYPE body_nodes = (
        NT_STR_MLN | NT_SET_MLS | NT_SET_DCT | NT_SET_LST | NT_SET_ROL |
        NT_SET_NIL | NT_STR_ROL | NT_COMMENT
    );

    constexpr NT_TYPE navy_nodes = (
        NT_LST_ROL  | NT_SET_MLS |
        NT_LST_DCT  | NT_SET_NIL |
        NT_LST_LST  | NT_SET_DCT |
        NT_LST_MLS  | NT_SET_LST |
        NT_LST_NIL  | NT_SET_ROL |
        NT_TAG_MLS
    );

    constexpr NT_TYPE blue_nodes = (
        NT_KEY_NIL | NT_KEY_MLS | NT_KEY_LST | NT_KEY_DCT | NT_KEY_ROL
    );

    constexpr NT_TYPE olive_nodes = NT_TAG_COM | NT_COMMENT;

    constexpr NT_TYPE no_newline = (
        NT_TAG_COM | NT_TAG_MLS | NT_LST_ROL | NT_KEY_NIL | NT_KEY_MLS |
        NT_KEY_LST | NT_KEY_DCT | NT_KEY_ROL | NT_SET_ROL
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
    size_t input_size;
    char *input_data = read_file_to_memory("../ugly.nt", &input_size);

    if (!input_data) {
        fprintf(stderr, "%s\n", "failed to read the input file");
        return EXIT_FAILURE;
    }

    NT_PARSER parser = nt_make_parser();
    nt_parser_set_blacklist(&parser, NT_SPACE|NT_NEWLINE);

    if (nt_parser_parse(&parser, input_data, 0) > (int) parser.mem.capacity) {
        fprintf(stderr, "not enough memory for %lu nodes\n", parser.doc.length);
        return free_and_return(input_data, EXIT_FAILURE);
    }

    return free_and_return(input_data, pretty_print(parser.doc.root, 0));
}
