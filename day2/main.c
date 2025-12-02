#define FILELIB_IMPLEMENTATION
#include"file.h"

#include "lib.h"

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "main"
#include "log.h"

const static char *INPUT_FILENAME = "./day2/input.txt";

int main() {
    long file_size = get_file_size(INPUT_FILENAME);
    if (file_size < 0) {
        LOG_ERROR("Failed to get file size for %s", INPUT_FILENAME);
        return -1;
    }

    LOG_INFO("Input file size of %ld bytes", file_size);

    InputContainer ctx = {(size_t)file_size, malloc((size_t)file_size + 1)};

    if (file_process_lines(INPUT_FILENAME, get_first_line, &ctx) != 0) {
        LOG_ERROR("file_process_lines failed");
        return -1;
    }

    LOG_INFO("Analyzing input: %s",ctx.input);

    long long sum = process(ctx.input);
    if (sum < 0) {
        LOG_ERROR("process failed");
        free(ctx.input);
        return -1;
    }

    fprintf(stderr, "\n");
    LOG_INFO("Final sum of antimirrored numbers: %lld", sum);

    free(ctx.input);
    return 0;
}
