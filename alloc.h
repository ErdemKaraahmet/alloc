/*
A C11 memory allocator implementation for Linux using `brk`/`sbrk`, based on
Marwan Burelle's "A Malloc Tutorial" (EPITA/LSE, 2009).
*/
#pragma once

#include <stddef.h>

void *malloc(size_t size);

void *calloc(size_t nmemb, size_t size);

void free(void *ptr);

void *realloc(void *ptr, size_t size);

void *reallocf(void *ptr, size_t size);
