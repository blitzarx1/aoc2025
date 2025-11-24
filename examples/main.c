#include<stdio.h>
#include<assert.h>

#define ARRAY_IMPLEMENTATION
#include"array.h"

#define HASHMAP_IMPLEMENTATION
#include"hashmap.h"

#define FILELIB_IMPLEMENTATION
#include"file.h"

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "main"
#include "log.h"

int check_arr() {
    Array *arr = arrnew(sizeof(int), 0);

    int val3 = 3;
    if (arrappend(arr, &val3)<0) {
        return -1;
    };

    int val4 = 4;
    if (arrappend(arr, &val4)<0) {
        return -1;
    };

    int val5 = 5;
    if (arrappend(arr, &val5)<0) {
        return -1;
    };

    LOG_INFO("el1: %d", ARR_GET_VAL(int, arr, 0));
    assert(ARR_GET_VAL(int, arr, 0) == 3);
    LOG_INFO("el2: %d", ARR_GET_VAL(int, arr, 1));
    assert(ARR_GET_VAL(int, arr, 1) == 4);
    LOG_INFO("el2: %d", ARR_GET_VAL(int, arr, 2));
    assert(ARR_GET_VAL(int, arr, 2) == 5);

    arrfree(arr);
    return 0;
}

// basic overwrite & independent keys
static void test_basic_ops(void) {
    HashMap *map = mapnew(0);
    assert(map != NULL);

    // overwrite same key multiple times
    assert(mapset(map, "key1", "value1") == 0);
    assert(mapset(map, "key1", "value2") == 0);
    assert(mapset(map, "key1", "value3") == 0);

    char *v1 = MAP_GET(char, map, "key1");
    assert(v1 != NULL);
    assert(strcmp(v1, "value3") == 0);  // last write wins

    // second independent key
    assert(mapset(map, "key2", "value4") == 0);
    char *v2 = MAP_GET(char, map, "key2");
    assert(v2 != NULL);
    assert(strcmp(v2, "value4") == 0);

    // key that does not exist
    char *v_missing = MAP_GET(char, map, "no_such_key");
    assert(v_missing == NULL);

    mapfree(map);
}

// insert enough keys to force growth & rehash, then verify all
static void test_resize_and_rehash(void) {
    HashMap *map = mapnew(2);   // small cap to trigger early growth
    assert(map != NULL);

    // insert a bunch of keys to cross load factor multiple times
    const int N = 100;
    char keys[N][16];
    char vals[N][16];

    for (int i = 0; i < N; i++) {
        // keys: "key0", "key1", ...
        // vals: "val0", "val1", ...
        snprintf(keys[i], sizeof(keys[i]), "k%d", i);
        snprintf(vals[i], sizeof(vals[i]), "v%d", i);

        assert(mapset(map, keys[i], vals[i]) == 0);
    }

    // verify everything is still there after resizes
    for (int i = 0; i < N; i++) {
        char *v = MAP_GET(char, map, keys[i]);
        assert(v != NULL);
        assert(strcmp(v, vals[i]) == 0);
    }

    // overwrite one of the existing keys after resize
    assert(mapset(map, "k10", "v10-updated") == 0);
    char *v10 = MAP_GET(char, map, "k10");
    assert(v10 != NULL);
    assert(strcmp(v10, "v10-updated") == 0);

    mapfree(map);
}

// a tiny sanity test for NULL-like behavior (defensive)
static void test_null_behavior(void) {
    // mapget with NULL map should just return NULL, not crash
    char *v = MAP_GET(char, NULL, "key");
    assert(v == NULL);

    // mapget with NULL key should just return NULL, not crash
    HashMap *map = mapnew(0);
    assert(map != NULL);
    v = MAP_GET(char, map, NULL);
    assert(v == NULL);
    mapfree(map);
}

int check_hashmap(void) {
    test_basic_ops();
    test_resize_and_rehash();
    test_null_behavior();
    return 0;
}

int line_handler(const char *line, void *ctx) {
    int *count = (int *)ctx;
    (*count)++;

    LOG_INFO("Line %d: %s", *count, line);

    return 0;
}

int check_file() {
    // simple line handler that just counts lines
    int line_count = 0;

    const char *filename = "examples/test_input.txt";
    int rc = file_process_lines(filename, line_handler, &line_count);
    if (rc < 0) {
        LOG_ERROR("file_process_lines failed");
        return -1;
    }

    LOG_INFO("Total lines processed: %d", line_count);

    return 0;
}

int main() {
    int err;

    if ((err = check_arr()) < 0) return -1;

    if ((err = check_hashmap()) < 0) return err;

    if ((err = check_file()) < 0) return err;

    return 0;
}
