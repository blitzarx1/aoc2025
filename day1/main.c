#define FILELIB_IMPLEMENTATION
#include"file.h"

#include "lib.h"

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "main"
#include "log.h"

const static char *INPUT_TEST_FILENAME = "./day1/input.txt";

int main() {
    State ctx = {START_POS, 0};

    int rc = file_process_lines(INPUT_TEST_FILENAME, input_line_handler, &ctx);
    if (rc < 0) {
        LOG_ERROR("file_process_lines failed");
        return -1;
    }

    fprintf(stderr, "\n");

    LOG_INFO("Final position: %d; Total zeroes reached: %d", ctx.position, ctx.zeroed);

    return 0;
}
