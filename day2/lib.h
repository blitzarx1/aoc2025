#include <string.h>
#include <stdbool.h>

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "lib"
#include "log.h"

typedef struct {
    size_t size;
    char *input;
} InputContainer;

static bool check_chunks(char *digit, size_t len, size_t chunk_size)
{
    int chunks = len / chunk_size;
    for (int i=0; i < chunk_size; i++) {
        char c = digit[i];

        for (int j=1; j < chunks; j++) {
            int pos = j * chunk_size + i;
            char c2 = digit[pos];
            if (c != c2) {
                LOG_DEBUG("Mismatch at chunk %d position %d: %c != %c", j, i, c, c2);
                return false;
            }
        }

    }

    return true;
};

static bool check_chunks_rec(char *digit, size_t len, size_t chunk_size) {
    if (chunk_size == 0) {
        LOG_DEBUG("Reached chunk_size 0, returning false");
        return false;
    }

    if (len % chunk_size != 0) {
        LOG_DEBUG("Length %zu not divisible by chunk_size %zu, propagating rec call...", len, chunk_size);
        return check_chunks_rec(digit, len, chunk_size - 1);
    }

    if (check_chunks(digit, len, chunk_size)) {
        LOG_INFO("Found antimirrored digit %s with chunk_size %zu", digit, chunk_size);
        return true;
    }

    return check_chunks_rec(digit, len, chunk_size - 1);
}

static bool is_antimirrored(char *digit, size_t len) {
    if (len < 2) {
        LOG_DEBUG("Length %zu less than 2, cannot be antimirrored", len);
        return false;
    }

    if (len % 2 != 0) return check_chunks_rec(digit, len, (len/2)+1);

    return check_chunks_rec(digit, len, len/2);
}

static int get_first_line(const char *line, void *ctx) {
    InputContainer *input = (InputContainer *)ctx;

    memcpy(input->input, line, strlen(line) + 1);

    return 0;
};

static char *itoa_alloc(long long value) {
    int len = snprintf(NULL, 0, "%lld", value);
    char *buf = malloc(len + 1);
    snprintf(buf, len + 1, "%lld", value);
    return buf;
}

static long long process_range(const char *range) {
    long long sum = 0;

    char *strstart = strtok((char *)range, "-");
    if (strstart == NULL) {
        LOG_ERROR("No digits found in range: %s", range);
        return -1;
    }
    long long start = strtoll(strstart, NULL, 10);

    char *strend = strtok(NULL, "-");
    if (strend == NULL) {
        LOG_ERROR("No end digit found in range: %s", range);
        return -1;
    }
    long long end = strtoll(strend, NULL, 10);

    long long i = start;
    for (;;) {
        if (i > end) {
            break;
        }

        char *digit = itoa_alloc(i);
        if (is_antimirrored(digit, strlen(digit))) {
            sum+=i;
        }

        free(digit);
        i++;
    }

    return sum;
}


long long process(char *line) {
    long long sum=0;

    char *rangetok;
    char *range = strtok_r(line, ",", &rangetok);
    if (range == NULL) {
        LOG_ERROR("No ranges found in input");
        return -1;
    }

    while (range != NULL) {
        fprintf(stderr, "\n");
        LOG_INFO("Processing range: %s", range);

        long long range_sum = process_range(range);
        if (range_sum < 0) {
            LOG_ERROR("process_range failed for range: %s", range);
            return -1;
        }

        LOG_INFO("Sum of antimirrored digits in range: %lld", range_sum);

        sum += range_sum;

        LOG_INFO("Total sum so far: %lld", sum);

        range = strtok_r(NULL, ",", &rangetok);
    };

    return sum;
}
