#include <stdbool.h>

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "lib"
#include "log.h"

#define ARRAY_IMPLEMENTATION
#include <array.h>

const static char *NOT_EMPTY_EL = "@";
const static char *REPLACED_EL = "x";

typedef struct {
    Array *map;
} Ctx;

int accumulate_map(const char *line, void *ctx){
    Ctx *map = (Ctx *)ctx;

    Array *arr_line = arrnew(sizeof(char), strlen(line));

    for (int i=0; i<strlen(line); i++){
        char c = line[i];
        if (arrappend(arr_line, &c)!=0){
            LOG_ERROR("arrappend failed");
            return -1;
        }
    }
    if (arrappend(map->map, &arr_line)!=0){
        LOG_ERROR("arrappend failed");
        return -1;
    }

    return 0;
}

static char *get_el(Array *map, int row, int col) {
    Array **line = ARR_GET(Array *, map, row);
    if (!line) {
        return NULL;
    }
    return ARR_GET(char, *line, col);
};

static bool check_el(Array *map, int row, int col) {
    Array *elrow = ARR_GET_VAL(Array *, map, row);
    int rowlen = arrlen(elrow);

    char *el = get_el(map, row, col);
    if (!el) return false;

    if (*el != *NOT_EMPTY_EL) return false;
    
    int not_empty_cnt = 0;
    
    char *elup = get_el(map, row - 1, col);
    if (elup && *elup == *NOT_EMPTY_EL) not_empty_cnt++;

    char *eldown = get_el(map, row + 1, col);
    if (eldown && *eldown == *NOT_EMPTY_EL) not_empty_cnt++;

    char *elleft = get_el(map, row, col - 1);
    if (elleft && *elleft == *NOT_EMPTY_EL) not_empty_cnt++;

    char *elright = get_el(map, row, col + 1);
    if (elright && *elright == *NOT_EMPTY_EL) not_empty_cnt++;

    char *elupleft = get_el(map, row - 1, col - 1);
    if (elupleft && *elupleft == *NOT_EMPTY_EL) not_empty_cnt++;

    char *elupright = get_el(map, row - 1, col + 1);
    if (elupright && *elupright == *NOT_EMPTY_EL) not_empty_cnt++;

    char *eldownleft = get_el(map, row + 1, col - 1);
    if (eldownleft && *eldownleft == *NOT_EMPTY_EL) not_empty_cnt++;

    char *eldownright = get_el(map, row + 1, col + 1);
    if (eldownright && *eldownright == *NOT_EMPTY_EL) not_empty_cnt++;

    return not_empty_cnt < 4;
};

int process_map(Array *map) {
    fprintf(stderr, "\n");

    int sum = 0;
    for (size_t i=0; i< arrlen(map); i++){
        Array *row = ARR_GET_VAL(Array *, map, i);

        for (size_t j=0; j< arrlen(row); j++){
            char *el = get_el(map, i, j);
            if (!el){
                LOG_ERROR("get_el returned NULL for (%zu, %zu)", i, j);
                return -1;
            }

            if (check_el(map, i, j)){
                sum++;
                if (arrset(row, j, REPLACED_EL) !=0){
                    LOG_ERROR("arrset failed");
                    return -1;
                }
                fprintf(stderr, "%c", *REPLACED_EL);
            } else {
                fprintf(stderr, "%c", *el);
            }

        }

        fprintf(stderr, "\n");
    }

    fprintf(stderr, "\n");

    return sum;
}
