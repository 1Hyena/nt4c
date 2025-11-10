// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

int main(int, char **) {
    constexpr size_t node_count = 8;
    NT_NODE nodes[node_count];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, node_count);

    if (nt_parse("hello: world\n", 0, &parser) > (int) node_count) {
        fprintf(
            stderr, "insufficient memory for %lu nodes\n", parser.node.count
        );

        return EXIT_FAILURE;
    }

    for (size_t i=0; i<parser.node.count; ++i) {
        NT_NODE *node = &parser.memory.nodes[i];

        if (node->data) {
            printf("%.*s", (int) node->size, node->data);
        }
    }

    return EXIT_SUCCESS;
}
