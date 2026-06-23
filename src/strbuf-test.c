// vim: set colorcolumn=85
// vim: fdm=marker

#include "koh_strbuf.h"
#include "munit.h"
#include <assert.h>
#include <limits.h>
#include <memory.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool verbose_print = true;

static MunitResult test_init_shutdown(
    const MunitParameter params[], void* data
) {
    StrBuf s = strbuf_init(NULL);
    strbuf_shutdown(&s);
    return MUNIT_OK;
}

static MunitResult test_add(
    const MunitParameter params[], void* data
) {
    StrBuf s = strbuf_init(NULL);

    char *lines[] = {
        "",
        "1",
        "12",
        "1234",
        "bebeb",
        NULL,
    };

    for (int i = 0; lines[i]; i++) {
        strbuf_add(&s, lines[i]);
    }

    for (int i = 0; i < s.num; i++) {
        munit_assert(strcmp(s.s[i], lines[i]) == 0);
    }
    /*printf("test_add: s.num %d\n", s.num);*/
    munit_assert_int(s.num, ==, 5);

    strbuf_shutdown(&s);
    return MUNIT_OK;
}

static MunitResult test_first_last(
    const MunitParameter params[], void* data
) {
    StrBuf s = strbuf_init(NULL);

    munit_assert(strbuf_first(&s) == NULL);
    munit_assert(strbuf_last(&s) == NULL);

    strbuf_add(&s, "1");

    munit_assert(strcmp(strbuf_first(&s), "1") == 0);
    munit_assert(strcmp(strbuf_last(&s), "1") == 0);

    strbuf_add(&s, "2");

    munit_assert(strcmp(strbuf_first(&s), "1") == 0);
    munit_assert(strcmp(strbuf_last(&s), "2") == 0);

    strbuf_add(&s, "3");

    munit_assert(strcmp(strbuf_first(&s), "1") == 0);
    munit_assert(strcmp(strbuf_last(&s), "3") == 0);

    strbuf_shutdown(&s);

    return MUNIT_OK;
}

static MunitResult test_addf(
    const MunitParameter params[], void* data
) {
    StrBuf s = strbuf_init(NULL);

    strbuf_addf(&s, "%d", 0);
    strbuf_addf(&s, "%d", -1);
    strbuf_addf(&s, "her %d", 9);

    /*
    for (int i = 0; i < s.num; i++) {
        printf("'%s'\n", s.s[i]);
    }
    */

    munit_assert_not_null(s.s);
    munit_assert(strcmp(s.s[0], "0") == 0);
    munit_assert(strcmp(s.s[1], "-1") == 0);
    munit_assert(strcmp(s.s[2], "her 9") == 0);
    munit_assert_null(s.s[3]);

    strbuf_shutdown(&s);
    return MUNIT_OK;
}

static MunitResult test_concat(
    const MunitParameter params[], void* data
) {

    {
        StrBuf s = strbuf_init(NULL);

        strbuf_add(&s, "1");
        strbuf_add(&s, "2");
        strbuf_add(&s, "3");
        strbuf_add(&s, "4");
        strbuf_add(&s, "5");

        char *line = strbuf_concat_alloc(&s, NULL);
        munit_assert_not_null(line);

        munit_assert(strcmp(line, "12345") == 0);

        free(line);
        strbuf_shutdown(&s);
    }

    {
        StrBuf s = strbuf_init(NULL);

        strbuf_add(&s, "1");
        strbuf_add(&s, "2");
        strbuf_add(&s, "3");
        strbuf_add(&s, "4");
        strbuf_add(&s, "5");

        char *line = strbuf_concat_alloc(&s, " ");
        munit_assert_not_null(line);

        printf("line '%s'\n", line);
        munit_assert(strcmp(line, "1 2 3 4 5") == 0);

        free(line);
        strbuf_shutdown(&s);
    }

    return MUNIT_OK;
}

// strbuf_clear — очистка без освобождения памяти
static MunitResult test_clear(
    const MunitParameter params[], void* data
) {
    StrBuf s = strbuf_init(NULL);

    strbuf_add(&s, "a");
    strbuf_add(&s, "b");
    strbuf_add(&s, "c");
    munit_assert_int(s.num, ==, 3);
    munit_assert_not_null(s.s);

    strbuf_clear(&s);
    munit_assert_int(s.num, ==, 0);
    munit_assert_null(strbuf_first(&s));
    munit_assert_null(strbuf_last(&s));

    strbuf_add(&s, "x");
    strbuf_add(&s, "y");
    munit_assert_int(s.num, ==, 2);
    munit_assert(strcmp(strbuf_first(&s), "x") == 0);
    munit_assert(strcmp(strbuf_last(&s), "y") == 0);

    strbuf_shutdown(&s);
    return MUNIT_OK;
}

// Обёртка для проверки strbuf_add_va
static void _add_va_wrap(StrBuf *s, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    strbuf_add_va(s, fmt, ap);
    va_end(ap);
}

// strbuf_add_va — форматирование из va_list
static MunitResult test_add_va(
    const MunitParameter params[], void* data
) {
    StrBuf s = strbuf_init(NULL);

    _add_va_wrap(&s, "%d", 42);
    _add_va_wrap(&s, "%s %d", "line", 7);
    _add_va_wrap(&s, "%.2f", 3.14f);

    munit_assert_int(s.num, ==, 3);
    munit_assert(strcmp(s.s[0], "42") == 0);
    munit_assert(strcmp(s.s[1], "line 7") == 0);
    munit_assert(strcmp(s.s[2], "3.14") == 0);
    munit_assert_null(s.s[3]);

    strbuf_shutdown(&s);
    return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {

    {
        "/test_first_last",
        test_first_last,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },

    {
        "/test_addf",
        test_addf,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },


    {
        "/test_add",
        test_add,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },

    {
        "/test_concat",
        test_concat,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },

    {
        "/test_init_shutdown",
        test_init_shutdown,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },

    {
        "/test_clear",
        test_clear,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },

    {
        "/test_add_va",
        test_add_va,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },

    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite test_suite = {
    "strbuf",
    test_suite_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE,
    .verbose = &verbose_print,
};

int main(int argc, char **argv) {
    return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}
