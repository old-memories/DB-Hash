#include "hash_func.h"

const size_t prime_numbers[PRIME_NUMBERS] = {
    53ul,
    97ul,
    193ul,
    389ul,
    769ul,
    1543ul,
    3079ul,
    6151ul,
    12289ul,
    49157ul,
    98317ul,
    196613ul,
    393241ul,
    786433ul,
    1572869ul,
    3145739ul,
    6291469ul,
    12582917ul,
    25165843ul,
    50331653ul,
    100663319ul,
    201326611ul,
    402653189ul,
    805306457ul,
    1610612741ul,
};


size_t
round_to_prime_number(size_t number)
{
    size_t res = 0;
    size_t i;
    for(i = 0; i < PRIME_NUMBERS; ++i){
        if(number < prime_numbers[i]){
            res = prime_numbers[i];
            break;
        }
    }

    return res;
}

size_t
hash_func_number(const void *key, size_t key_len, size_t size)
{
    size_t number = *(size_t *)(key);
    return number % size;
}

size_t
hash_func_string(const void *key, size_t key_len, size_t size)
{
    const char *_key = *((const char **)key);
    size_t seed = 1313131;
    size_t h = 0;
    while(key_len--){
        h = h * seed + (*_key++);
    }
    return h % size;
}