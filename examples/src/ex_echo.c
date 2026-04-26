// SPDX-License-Identifier: MIT
#include "utils.h"
#include "../../nt4c.h"
#include <stdlib.h>


int main(int, char **) {
    size_t input_size;
    char *input_data = read_file_to_memory("../minimal.nt", &input_size);

    if (!input_data) {
        fprintf(stderr, "%s\n", "failed to read the input file");
        return EXIT_FAILURE;
    }

    int node_count = nt_parse(input_data, input_size, nullptr);

    if (node_count <= 0) {
        fprintf(stderr, "%s\n", "parse error");
        return free_and_return(input_data, EXIT_FAILURE);
    }

    NT_NODE nodes[node_count];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, sizeof(nodes)/sizeof(nodes[0]));

    if (nt_parse(input_data, input_size, &parser) > (int) node_count) {
        fprintf(stderr, "not enough memory for %lu nodes\n", parser.doc.length);

        return free_and_return(input_data, EXIT_FAILURE);
    }

    for (NT_NODE *it = parser.doc.begin; it < parser.doc.end; ++it) {
        printf("%.*s", (int) it->size, it->data);
    }

    return free_and_return(input_data, EXIT_SUCCESS);
}
