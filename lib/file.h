#ifndef FILELIB_H
#define FILELIB_H

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "file"
#include "log.h"

// Line handler will be called for every line by `process_lines`
//
// Should return:
// - 0 to continue
// - nonzero to stop early
//
// Nonzero return codes will be propagated back to the caller of `process_lines`.
typedef int (*file_line_handler)(const char *line, void *ctx);

// Returns:
// - 0 on full success
// - nonzero if handler stopped early
// - -1 if the file could not be opened or memory allocation failed.
int file_process_lines(const char *filename, file_line_handler fn, void *ctx);

#ifdef FILELIB_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>

// Reads one logical line into a reusable buffer (grows as needed).
// Strips trailing '\n' and optional '\r'. 
//
// Returns:
// - 1 = line read
// - 0 = EOF (no chars)
// - -1 = allocation failure
static int read_line_reuse(FILE *f, char **buf, size_t *cap, size_t *len) {
    int ch;
    size_t n = 0;

    if (*buf == NULL || *cap == 0) {
        *cap = 128;
        *buf = (char*)malloc(*cap);
        if (!*buf) return -1;
    }

    // Read until newline or EOF, expanding buffer as required
    while ((ch = fgetc(f)) != EOF) {
        if (n + 1 >= *cap) {
            size_t newcap = (*cap < 16384) ? (*cap * 2) : (*cap + *cap / 2);
            char *nb = (char*)realloc(*buf, newcap);
            if (!nb) return -1;
            *buf = nb;
            *cap = newcap;
        }
        if (ch == '\n') break;
        (*buf)[n++] = (char)ch;
    }

    // No characters and EOF → no more lines
    if (ch == EOF && n == 0) return 0;

    // Trim CR from CRLF files
    if (n > 0 && (*buf)[n - 1] == '\r') n--;

    (*buf)[n] = '\0'; // NULL terminate
    if (len) *len = n;
    return 1;
}

int file_process_lines(const char *filename, file_line_handler fn, void *ctx) {
    LOG_DEBUG("processing lines from file: %s", filename);

    FILE *file = fopen(filename, "r");
    if (!file) return -1;

    int rc = 0;
    char *line = NULL;
    size_t cap = 0, len = 0;

    for (;;) {
        int r = read_line_reuse(file, &line, &cap, &len);
        if (r == 0) break;             // EOF
        if (r < 0) { rc = -1; break; } // alloc failure

        rc = fn(line, ctx);
        if (rc != 0) break;            // stop early, propagate handler code
    }

    free(line);
    fclose(file);

    LOG_DEBUG("finished processing lines from file: %s", filename);

    return rc;
}

#endif

#endif
