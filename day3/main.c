#include<stdio.h>

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "main"
#include "log.h"

#include "lib.h"

const static char *INPUT_FILENAME = "./day3/input.txt";

const static int NUM_SIZE = 12;

int main() {
    Ctx ctx = {0, NUM_SIZE};

    if (file_process_lines(INPUT_FILENAME, process_line, &ctx) !=0) {
        LOG_ERROR("file_process_lines failed");
        return -1;
    };

    fprintf(stderr, "\n");

    LOG_INFO("Final sum: %lld", ctx.sum);

    return 0;
}
