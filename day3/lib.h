#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "lib"
#include "log.h"

#define FILELIB_IMPLEMENTATION
#include<file.h>

#include<string.h>

typedef struct {
    long long sum;
    int num_size;
} Ctx;

char *get2max_char(const char *s, int n) {
    if (s == NULL) {
        return NULL;
    }

    int len = strlen(s);
    if (len == 0) {
        LOG_ERROR("Empty string passed");
        return NULL;
    }

    int rb = len - n;
    if (rb < 0) {
        LOG_ERROR("String too short for %d max chars: %s", n, s);
        return NULL;
    }
    int lb=0;

    char *buf = malloc(sizeof(char) * n);

    for (int i = 0; i < n; i++) {
        buf[i] = 0;

        char max = 0;
        LOG_DEBUG("Finding max char for position %d between %d and %d", i, lb, rb);
        for (int j = lb; j <= rb; j++) {
            LOG_DEBUG("\tComparing char %c at pos %d with current max %c", s[j], j, max);
            if (s[j] > max) {
                max = s[j];
                lb = j + 1;
            }
        }

        rb++;

        LOG_INFO("Selected max char %c for position %d", max, i);
        buf[i] = max;
    }

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
    if ((maxchars = get2max_char(line, c->num_size)) == NULL) {
        LOG_ERROR("get2max_char failed");
        return -1;
    }

    LOG_INFO("Found max chars: %s", maxchars);

    c->sum += strtoll((char *)maxchars, NULL, 10);
    free(maxchars);

    return 0;
};

