// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>

int main(int, char **) {
    NT_PARSER parser = {};

    if (nt_parse("hello world", 0, &parser) <= 0) {
        return EXIT_FAILURE;
    }

    printf("%.*s\n", (int) parser.doc.begin->size, parser.doc.begin->data);

    return EXIT_SUCCESS;
}
