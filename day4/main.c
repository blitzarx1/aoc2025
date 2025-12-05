#include<stdio.h>

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "main"
#include "log.h"

#include "lib.h"

#define FILELIB_IMPLEMENTATION
#include <file.h>

const static char* INPUT_FILENAME = "./day4/input.txt";

int main() {
    Ctx ctx = {arrnew(sizeof(Array *), 100)};

    if (file_process_lines(INPUT_FILENAME, accumulate_map, &ctx) !=0) {
        LOG_ERROR("accumulate_map failed");
        return -1;
    };

    int sum = 0;
    for (;;) {
        int cur_sum = process_map(ctx.map);
        if (cur_sum == 0) break;

        sum += cur_sum;
    }

    LOG_INFO("Final sum: %d", sum);

    return 0;
}
