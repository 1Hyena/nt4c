// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>


int main(int, char **) {
    NT_NODE nodes[32];
    NT_PARSER parser = {};

    nt_parser_set_memory(&parser, nodes, (sizeof(nodes) / sizeof((nodes)[0])));
    nt_parse("test: 1", 0, &parser);

    return EXIT_SUCCESS;
}
