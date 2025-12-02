#include <stdlib.h>
#include "lib.h"

#include <stdbool.h>

#define ACUTEST_MAIN
#include "acutest.h"

static void check_is_antimirrored( const char *digit,
                              size_t len,
                              bool expected)
{
    int res = is_antimirrored((char *)digit, len);
    TEST_CHECK_(res == expected,
                "is_antimirrored(\"%s\", %zu): expected %s, got %s",
                digit, len, expected?"true":"false", res?"true":"false");
}

void test_simple(void)
{
    check_is_antimirrored("123123", 6, true);
    check_is_antimirrored("123124", 6, false);
    check_is_antimirrored("121212", 6, true);
    check_is_antimirrored("1", 1, false);
    check_is_antimirrored("12312", 5, false);
    check_is_antimirrored("123", 3, false);
}

TEST_LIST = {
    { "simple",        test_simple },
    { NULL, NULL }
};

