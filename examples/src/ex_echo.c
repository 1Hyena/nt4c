// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

const char input_data[] = {
#embed "../repository.nt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
    , '\0'
};

static void dump_node(NT_NODE *node, size_t depth) {
    if (node->data) {
        printf("%.*s", (int) node->size, node->data);
        ++depth;
    }

    for (NT_NODE *child = node->children; child; child = child->next) {
        dump_node(child, depth);
    }
}

int main(int, char **) {
    constexpr size_t node_count = 128;
    NT_NODE nodes[node_count];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, node_count);

    if (nt_parse(input_data, 0, &parser) > (int) node_count) {
        fprintf(
            stderr, "insufficient memory for %lu nodes\n", parser.node.count
        );
    }
    else dump_node(parser.node.root, 0);

    return EXIT_SUCCESS;
}
