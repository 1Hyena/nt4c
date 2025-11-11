// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

int main(int, char **) {
    NT_PARSER parser = {};

    if (nt_parse("hello world", 0, &parser) > (int) parser.memory.capacity) {
        fprintf(
            stderr, "insufficient memory for %lu nodes\n", parser.node.count
        );

        return EXIT_FAILURE;
    }

    NT_NODE *content = parser.nest.begin;

    if (content) {
        printf("%.*s\n", (int) content->size, content->data);
    }

    return EXIT_SUCCESS;
}
