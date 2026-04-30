// SPDX-License-Identifier: MIT
#include "utils.h"
#include "../../nt4c.h"
#include <stdlib.h>


static int callback(NT_TYPE t, const char *str, size_t size, void *, size_t d) {
    for (size_t i=1; i<d; ++i) {
        printf("%s", "    ");
    }

    t = nt_type_type(t); // Let's only print the type codes of node groups.

    if (t == NT_NEWLINE) {
        printf("%s (depth %lu)\n", nt_type_code(t), d);
    }
    else {
        printf(
            "%s => [%.*s] (depth %lu)\n", nt_type_code(t), (int) size, str, d
        );
    }

    return 0;
}

int main(int, char **) {
    size_t input_size;
    char *input_data = read_file_to_memory("../minimal.nt", &input_size);

    if (!input_data) {
        fprintf(stderr, "%s\n", "failed to read the input file");
        return EXIT_FAILURE;
    }

    nt_parse(input_data, input_size, callback, nullptr);

    return free_and_return(input_data, EXIT_SUCCESS);
}
