#ifndef CONF_H
#define CONF_H

#include <stddef.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#define NAME_LEN 32
#define ADDR_LEN 32

typedef struct conf {
    char name[NAME_LEN];
    int server;
    char addr[ADDR_LEN];
    size_t port;
    size_t db_size;
    int create;
} conf_t;

extern void parse_conf(int argc, char **argv, conf_t *conf);

#endif