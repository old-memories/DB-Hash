#include "my_db.h"

static const hash_func_info_t hash_func_info_table[HASH_TYPES] = {
    {
        .name = "string",
        .type = HASH_STRING,
        .hash_func = hash_func_string,
        .key_cmp = key_string_cmp
    }
};

static TAILQ_HEAD(, db) g_db_list = TAILQ_HEAD_INITIALIZER(g_db_list);


db_t *
create_db(char *name, size_t size){
    db_t *db;
    int i;

    db = malloc(sizeof(db_t));
    if(db == NULL){
        return NULL;
    }
    memset(db, 0, sizeof(db_t));
    strcpy(db->name, name);
    db->capacity = size;
    db->info = &hash_func_info_table[HASH_STRING];
    db->table = malloc(db->capacity * sizeof(bucket_t));
    if(db->table == NULL){
        free(db);
        return NULL;
    }
    for (i = 0; i < db->capacity; i ++){
        db->table[i].head = NULL;
        db->table[i].size = 0;
        pthread_mutex_init(&(db->table[i].mutex), NULL);
        TAILQ_INIT(&(db->table[i].index_list));
    }

    TAILQ_INSERT_TAIL(&g_db_list, db, link);

    return db;
}


db_t *
open_db(char *name){
    
    db_t *db;
    
    if (name == NULL) {
        return NULL;
    }

    TAILQ_FOREACH(db, &g_db_list, link) {
       if (strcmp(db->name, name) == 0) {
           return db;
       }
    }

    return NULL;
}

int
close_db(db_t *db){
    return -1;
}


static int
db_set(db_t *db, void *key, size_t key_len, void *value, size_t value_len)
{
    bucket_t *b;
    size_t i;
    index_t *new_index, *index;

    if (key == NULL || key_len == 0) {
        return -1;
    }

    i = db->info->hash_func(key, key_len, db->capacity);

    b = &(db->table[i]);

    new_index = malloc(sizeof(index_t));
    if (new_index == NULL) {
        return -1;
    }

    new_index->key_len = key_len;

    new_index->key = malloc(key_len);
    if(new_index->key == NULL) {
        free(new_index);
        return -1;
    }

    memcpy(new_index->key, key, key_len);

    new_index->value_len = value_len;

    new_index->value = malloc(value_len);
    if (new_index->value == NULL) {
        free(new_index->key);
        free(new_index);
        return -1;
    }
    
    memcpy(new_index->value, value, value_len);

    pthread_mutex_lock(&(b->mutex));

    TAILQ_FOREACH(index, &b->index_list, link) {
       if (key_len == index->key_len && db->info->key_cmp(key, index->key, key_len) == 0) {
            TAILQ_REMOVE(&b->index_list, index, link);
            free(index->key);
            free(index->value);
            free(index);
            TAILQ_INSERT_HEAD(&b->index_list, new_index, link);
            pthread_mutex_unlock(&(b->mutex));
            return 0;
       }
    }

    TAILQ_INSERT_HEAD(&b->index_list, new_index, link);

    pthread_mutex_unlock(&(b->mutex));

    return 0;
}

static int
db_get(db_t *db, void *key, size_t key_len, void *value, size_t *value_len)
{
    bucket_t *b;
    size_t i;
    index_t *index;

    if (key == NULL || key_len == 0) {
        return -1;
    }

    i = db->info->hash_func(key, key_len, db->capacity);

    b = &(db->table[i]);    

    pthread_mutex_lock(&(b->mutex));

    TAILQ_FOREACH(index, &b->index_list, link) {
       if (key_len == index->key_len && db->info->key_cmp(key, index->key, key_len) == 0) {
           *value_len = index->value_len;
            memcpy(value, index->value, index->value_len);
            TAILQ_REMOVE(&b->index_list, index, link);
            TAILQ_INSERT_HEAD(&b->index_list, index, link);
            pthread_mutex_unlock(&(b->mutex));
            return 0;
       }
    }
    pthread_mutex_unlock(&(b->mutex));
    return -1;
}



static int
db_delete(db_t *db, void *key, size_t key_len)
{
    bucket_t *b;
    size_t i;
    index_t *index;

    if (key == NULL || key_len == 0) {
        return -1;
    }

    i = db->info->hash_func(key, key_len, db->capacity);

    b = &(db->table[i]);    

    pthread_mutex_lock(&(b->mutex));

    TAILQ_FOREACH(index, &b->index_list, link) {
       if (key_len == index->key_len && db->info->key_cmp(key, index->key, key_len) == 0) {
            TAILQ_REMOVE(&b->index_list, index, link);
            free(index->key);
            free(index->value);
            free(index);
            pthread_mutex_unlock(&(b->mutex));
            return 0;
       }
    }
    pthread_mutex_unlock(&(b->mutex));
    return -1;
}

int
db_run_cmd(db_t *db, db_data_t *data)
{
    int ret = 0;

    if (data == NULL) {
        return -1;
    }

    switch (data->cmd_type) {
        case SET: {
            ret = db_set(db, data->key, data->key_len, data->value, data->value_len);
            break;
        }
        case GET: {
            ret = db_get(db, data->key, data->key_len, data->value, &(data->value_len));
            break;
        }
        case DELETE: {
            ret = db_delete(db, data->key, data->key_len);
            break;
        }
        default: {
            ret = -1;
            break;
        }
    }

    return ret;

}