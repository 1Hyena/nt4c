#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


static inline const char *basename(const char *path) {
    const char *file = path;

    while (*path) {
        if (*path++ == '/') {
            file = path;
        }
    }

    return file;
}

static inline char* read_file_to_memory(
    const char *filename, size_t *filesize
) {
    // Reads a file into a memory buffer. Sets filesize to bytes read.
    // Returns a pointer to the buffer (caller must free), or NULL on error.

    if (filesize) {
        *filesize = 0;
    }

    FILE *f = fopen(filename, "rb");

    if (!f) {
        return (
            basename(filename) != filename ? (
                read_file_to_memory(basename(filename), filesize)
            ) : nullptr
        );
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (length < 0) {
        fclose(f);
        return nullptr;
    }

    // Allocate memory
    const size_t bufsz = (size_t) (length + 1);
    char *buffer = malloc(bufsz);

    if (!buffer) {
        fclose(f);
        return nullptr;
    }

    // Read content
    size_t read_size = fread(buffer, 1, bufsz, f);
    buffer[read_size] = '\0'; // Null-terminate
    fclose(f);

    if (filesize) {
        *filesize = read_size;
    }

    if (read_size + 1 != bufsz) {
        free(buffer);
        return nullptr;
    }

    return buffer;
}

static inline int free_and_return(void *data, int code) {
    free(data);
    return code;
}
