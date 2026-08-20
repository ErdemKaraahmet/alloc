#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "../alloc.h"

#define TEST_PASSED "\x1b[32mPASSED\x1b[0m"
#define COUNT 1000
#define ALIGNMENT _Alignof(max_align_t)

#define RUN_TEST(func)                                                         \
    do {                                                                       \
        printf("%s ", #func);                                                  \
        func();                                                                \
        printf(TEST_PASSED);                                                   \
        printf("\n");                                                          \
    } while (0)

void test_alignment(void);
void test_malloc(void);
void test_calloc(void);
void test_free(void);
void test_realloc(void);

int main(void) {

    printf("\nAllocations per test: %d\n\n", COUNT);

    RUN_TEST(test_alignment);
    RUN_TEST(test_malloc);
    RUN_TEST(test_calloc);
    RUN_TEST(test_free);
    RUN_TEST(test_realloc);

    printf("\n");

    return 0;
}

void test_malloc(void) {

    // Edge case: zero-byte allocation
    void *p_zero = malloc(0);
    free(p_zero);

    unsigned int *ptrs[COUNT];
    for (size_t i = 0; i < COUNT; i++) {
        size_t num_elements = (i + 1) * 32;
        ptrs[i] = malloc(num_elements * sizeof(unsigned int));

        assert(ptrs[i] != NULL);

        // Write to all
        for (size_t k = 0; k < num_elements; k++) {
            ptrs[i][k] = (unsigned int)(i * k * 1000);
        }
    }

    for (size_t i = 0; i < COUNT; i++) {
        size_t num_elements = (i + 1) * 32;
        for (size_t k = 0; k < num_elements; k++) {
            assert(ptrs[i][k] == (unsigned int)(i * k * 1000));
        }
    }

    for (size_t i = 0; i < COUNT; i++) {
        free(ptrs[i]);
    }
}

void test_calloc(void) {

    unsigned int *ptrs[COUNT];
    for (size_t i = 0; i < COUNT; i++) {
        size_t num_elements = (i + 1) * 32;
        ptrs[i] = calloc(num_elements, sizeof(unsigned int));

        assert(ptrs[i] != NULL);
    }

    for (size_t i = 0; i < COUNT; i++) {
        size_t num_elements = (i + 1) * 32;
        for (size_t k = 0; k < num_elements; k++) {
            assert(ptrs[i][k] == 0);
        }
    }

    for (size_t i = 0; i < COUNT; i++) {
        free(ptrs[i]);
    }
}

void test_free(void) {

    // Test block reuse
    void *p1 = malloc(128);
    void *p2 = malloc(128);
    assert(p1 != NULL && p2 != NULL);

    free(p1);

    void *p3 = malloc(128);
    assert(p3 == p1);

    free(p2);
    free(p3);

    // Test Fusion
    void *a = malloc(100);
    void *b = malloc(100);
    void *c = malloc(100);

    free(a);
    free(b);

    /* align(100) = 112 if your system aligns to a multiple of 16 meta-data
     * block struct is 48 bytes so a allocation between 0 and 272 bytes should
     * fit into fused a+b block*/

    void *d = malloc(272);
    assert(d == a);
    free(d);

    void *e = malloc(273);
    assert(e != a);
    free(e);

    free(c);
}

void test_realloc(void) {

    // current size big enough, just shrink/split
    void *a = malloc(100);
    void *b = malloc(100);

    void *s = realloc(a, 12);
    assert(s != NULL && s == a);

    /* if a = 0, b = 112 + 48. s total 48 + 16, empty space between b and s
     * is 86. s->size = 86 -BLOCK_SIZE = 38. therefore allocating less than 36
     * bytes should return a pointer between s and b */
    void *c = malloc(30);
    assert(s < c && c < b);
    free(c);

    // big enough when fused with ->next (c that is just freed)
    void *f = realloc(s, 100);
    assert(f != NULL && f == s);

    // fell back to malloc and check if data is preserved
    char *buffer = malloc(100);
    for (size_t i = 0; i < 100; i++) {
        buffer[i] = (char)i;
    }

    memcpy(f, buffer, 100);
    void *m = realloc(f, 200);
    assert(m != NULL && m != f && m != b);
    assert(memcmp(m, buffer, 100) == 0);

    free(b);
    free(m);
    free(buffer);

    // C standard edge cases
    void *p1 = realloc(
        NULL,
        50); // should return a new non-NULL pointer (behaves like malloc).
    assert(p1 != NULL);

    void *p2 = realloc(p1, 0); // should free ptr and return NULL.
    assert(p2 == NULL);
}

void test_alignment(void) {

    void *ptrs[COUNT];
    for (size_t i = 0; i < COUNT; i++) {
        ptrs[i] = malloc(rand() % 1000);
        // bitwise operators are defined only for integers.
        assert(((uintptr_t)ptrs[i] & (ALIGNMENT - 1)) == 0);
    }
    for (size_t i = 0; i < COUNT; i++) {
        free(ptrs[i]);
    }
}
