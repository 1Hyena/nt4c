// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

const char input_data[] = {
#embed "../minimal.nt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
    , '\0'
};

int main(int, char **) {
    int node_count = nt_parse(input_data, sizeof(input_data), nullptr);

    if (node_count <= 0) {
        fprintf(stderr, "%s\n", "parse error");
        return EXIT_FAILURE;
    }

    NT_NODE nodes[node_count];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, sizeof(nodes)/sizeof(nodes[0]));

    if (nt_parse(input_data, sizeof(input_data), &parser) > (int) node_count) {
        fprintf(stderr, "not enough memory for %lu nodes\n", parser.doc.length);

        return EXIT_FAILURE;
    }

    for (NT_NODE *it = parser.doc.begin; it < parser.doc.end; ++it) {
        printf("%.*s", (int) it->size, it->data);
    }

    return EXIT_SUCCESS;
}
