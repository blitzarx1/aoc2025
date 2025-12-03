#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "lib"
#include "log.h"

#define FILELIB_IMPLEMENTATION
#include<file.h>

typedef struct {
    long long sum;
} Ctx;

char *get2max_char(const char *s, int n) {
    if (s == NULL) {
        return NULL;
    }

    char *buf = malloc(sizeof(char) * n);

    // TODO: generalize for n
    char max1 = 0;
    char max2 = 0;

    int i = 0;
    int len = 1;
    int max1_idx = -1;
    int max2_idx = -1;
    for (;;) {
        char c = s[i];
        if (c == '\0') break;

        if (c > max1) {
            if (max1>max2) {
                max2 = max1;
                max2_idx = max1_idx;
            }

            max1 = c;
            max1_idx = i;
        } else {
            if ( c != max1 && c > max2 ) {
                max2 = c;
                max2_idx = i;
            }
        }

        i++;
        len++;
    }

    len--;

    LOG_DEBUG("Max1: %c at %d, Max2: %c at %d, len %d", max1, max1_idx, max2, max2_idx, len);

    if (max1_idx == len - 1) {
        max1 = max2;
        max1_idx = max2_idx;
    }

    i = max1_idx + 1;
    max2= s[i];
    for (int j = i; j < len; j++) {
        if (s[j] > max2) {
            max2 = s[j];
        }
    }

    buf[0] = max1;
    buf[1] = max2;

    // TODO: end ----

    return buf;
}

int process_line(const char *line, void *ctx) {
    fprintf(stderr, "\n");

    if (line == NULL) {
        LOG_ERROR("line is NULL");
        return -1;
    }

    LOG_INFO("Processing line: %s", line);

    Ctx *c = (Ctx *)ctx;

    char *maxchars;
    if ((maxchars = get2max_char(line, 2)) == NULL) {
        LOG_ERROR("get2max_char failed");
        return -1;
    }

    LOG_INFO("Found max chars: %s", maxchars);

    c->sum += strtoll((char *)maxchars, NULL, 10);
    free(maxchars);

    return 0;
};

