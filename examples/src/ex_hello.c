// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

static void dump_node(NT_NODE *node, size_t depth) {
    if (node->data) {
        for (size_t i=0; i<depth; ++i) {
            printf("%s", "    ");
        }

        printf("%.*s\n", (int) node->size, node->data);
        ++depth;
    }

    for (NT_NODE *child = node->children; child; child = child->next) {
        dump_node(child, depth);
    }
}

int main(int, char **) {
    constexpr size_t node_count = 8;
    NT_NODE nodes[node_count];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, node_count);
    nt_parse("hello: world", 0, &parser);
    dump_node(parser.node.root, 0);

    return EXIT_SUCCESS;
}
