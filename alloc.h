/*
Platform independent C99 malloc implementation
*/
#pragma once

struct s_block {
    size_t size;
    struct s_block *next;
    struct s_block *prev;
    int free;
    void *ptr;
    char data[];
};

typedef struct s_block *t_block;

void *malloc(const size_t size);

void *calloc(size_t nmemb, size_t size);

void free(void *ptr);

void *realloc(void *ptr, size_t size);

void *reallocf(void *ptr, size_t size);
