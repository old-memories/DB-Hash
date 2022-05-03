#ifndef __HASH_FUNC_H__
#define __HASH_FUNC_H__

#include <stddef.h>

#define PRIME_NUMBERS 25

typedef enum hash_type {
    HASH_NUM,
    HASH_STRING,
    HASH_TYPES
} hash_type_t;

typedef struct hash_func_info {
    char *name;
    hash_type_t type;
    size_t (*hash_func)(const void *key, size_t key_len, size_t size);
} hash_func_info_t;

extern const size_t prime_numbers[PRIME_NUMBERS];
extern size_t round_to_prime_number(size_t number);
extern size_t hash_func_number(const void *key, size_t key_len, size_t size);
extern size_t hash_func_string(const void *key, size_t key_len, size_t size);

#endif