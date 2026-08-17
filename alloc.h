/*
Custom C11 malloc implementation for Linux
*/
#pragma once

#include <stddef.h>

void *malloc(size_t size);

void *calloc(size_t nmemb, size_t size);

void free(void *ptr);

void *realloc(void *ptr, size_t size);

void *reallocf(void *ptr, size_t size);
