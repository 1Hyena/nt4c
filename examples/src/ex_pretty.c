// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

const char input_data[] = {
#embed "../repository.nt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
    , '\0'
};

static void pretty_print(NT_NODE *node, size_t depth) {
    if (node->data) {
        for (size_t i=0; i<depth; ++i) {
            printf("%s", "    ");
        }

        printf("%.*s\n", (int) node->size, node->data);
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
