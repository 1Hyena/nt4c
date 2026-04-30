// SPDX-License-Identifier: MIT
#include "../../nt4c.h"
#include <stdlib.h>


static int callback(NT_TYPE, const char *text, size_t size, void *, size_t) {
    printf("%.*s\n", (int) size, text);
    return -1;
}

int main(int, char **) {
    nt_parse("hello world", 0, callback, nullptr);

    return EXIT_SUCCESS;
}
