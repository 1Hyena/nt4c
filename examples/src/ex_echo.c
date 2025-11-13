// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

const char input_data[] = {
#embed "../multiline.nt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
    , '\0'
};

int main(int, char **) {
    constexpr size_t node_count = 200;
    NT_NODE nodes[node_count];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, node_count);

    if (nt_parse(input_data, 0, &parser) > (int) node_count) {
        fprintf(
            stderr, "insufficient memory for %lu nodes\n", parser.node.count
        );

        return EXIT_FAILURE;
    }
    const char *last_str = nullptr;
    for (NT_NODE *it = parser.nest.begin; it < parser.nest.end; ++it) {
        printf(it->type == NT_NEWLINE ? "*%.*s" : "[%.*s]", (int) it->size, it->data);

        if (it->data < last_str) { // TODO: remove this block
            abort();
        }
    }

    return EXIT_SUCCESS;
}
