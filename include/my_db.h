#ifndef MY_DB_H
#define MY_DB_H

#include <string.h>
#include <pthread.h>
#include <sys/queue.h>
#include"conf.h"
#include"hash_func.h"

#define MAX_KEY_LEN 1 << 10
#define MAX_VALUE_LEN 1 << 10

typedef enum cmd_type {
    SET,
    GET,
    DELETE,
} cmd_type_t;

typedef enum cmd_status {
    CMD_SUCCESS,
    CMD_NOENTRY,
    CMD_FAILURE,
} cmd_status_t;


typedef struct db_data {
    cmd_type_t cmd_type;
    cmd_status_t status;
    char key[MAX_KEY_LEN];
    size_t key_len; 
    char value[MAX_VALUE_LEN];
    size_t value_len;
} db_data_t;

typedef struct index {
    void *key;
    hash_type_t type;
    size_t key_len; 
    void *value;
    size_t value_len;
    struct index *next;
    TAILQ_ENTRY(index) link;
} index_t;

typedef struct bucket {
    index_t *head;
    size_t size;
    pthread_mutex_t mutex;
    TAILQ_HEAD(index_tailq_list, index) index_list;
} bucket_t;



typedef struct db {
    char name[NAME_LEN];
    size_t capacity;
    size_t size;
    const hash_func_info_t *info;
    bucket_t *table;
    TAILQ_ENTRY(db) link;
} db_t;


extern db_t *create_db(char *name, size_t size);
extern db_t *open_db(char *name);
extern int close_db(db_t *db);
extern int db_run_cmd(db_t *db, db_data_t *data);

#endif