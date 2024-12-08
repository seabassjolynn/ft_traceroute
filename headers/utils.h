#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#ifdef DEBUG
    #define DEBUG_LOG(fmt, ...) printf("DEBUG: " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_LOG(fmt, ...) // No operation (NOP) if DEBUG is not defined
#endif

size_t ft_strlen(const char *s);

int ft_strncmp(const char *s1, const char *s2, size_t n);

void *ft_memset(void *b, int c, size_t len);

void *ft_memcpy(void *dst, const void *src, size_t n);

#endif