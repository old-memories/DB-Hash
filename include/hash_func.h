#ifndef __HASH_FUNC_H__
#define __HASH_FUNC_H__

#include <stddef.h>
#include <string.h>

#define PRIME_NUMBERS 25

typedef enum hash_type {
    HASH_STRING,
    HASH_TYPES
} hash_type_t;

typedef struct hash_func_info {
    char *name;
    hash_type_t type;
    size_t (*hash_func)(const void *key, size_t key_len, size_t size);
    int (*key_cmp)(const void *k1, const void *k2, size_t key_len);
} hash_func_info_t;

extern size_t hash_func_string(const void *key, size_t key_len, size_t size);
extern int key_string_cmp(const void *k1, const void *k2, size_t key_len);
extern size_t round_to_prime_number(size_t number);


#endif