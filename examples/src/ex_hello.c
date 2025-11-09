// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

static void serialize(NT_NODE *node, size_t depth) {
    if (node->data) {
        for (size_t i=0; i<depth; ++i) {
            printf("%s", "    ");
        }

        printf("%.*s\n", (int) node->size, node->data);
        ++depth;
    }

    for (NT_NODE *child = node->children; child; child = child->next) {
        serialize(child, depth);
    }
}

int main(int, char **) {
    NT_NODE nodes[32];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, (sizeof(nodes) / sizeof((nodes)[0])));
    nt_parse("hello: world", 0, &parser);
    serialize(parser.node.root, 0);

    return EXIT_SUCCESS;
}
