#include "my_db.h"

static db_list_t db_list_head = {
    .db = NULL,
    .next = NULL
};

static int
add_new_db(db_list_t *db_list_head, db_t *new_db){
    db_list_t *new_db_node = malloc(sizeof(db_list_t));
    if(new_db_node == NULL){
        return -1;
    }
    new_db_node->db = new_db;
    new_db_node->next = db_list_head->next;
    db_list_head->next = new_db_node;
    return 0;
}

static int
delete_db(db_list_t *db_list_head, db_t *delete_db){
    db_list_t *node = db_list_head;
    while(node->next != NULL){
        if(node->next->db == delete_db){
            node->next = node->next->next;
            return 0;
        }
        node = node->next;
    }
    return -1;
}

static db_t *
find_db(db_list_t *db_list_head, char *name){
    db_list_t *node = db_list_head;
    while(node->next != NULL){
        if(strcmp(node->next->db->name, name) == 0){
           return node->next->db;
        }
        node = node->next;
    }
    return NULL;
}

db_t *
create_db(char *name, size_t size){
    db_t *db = malloc(sizeof(db_t));
    if(db == NULL){
        return NULL;
    }
    memset(db, 0, sizeof(db_t));
    strcpy(db->name, name);
    db->capacity = size;
    db->table = malloc(db->capacity * sizeof(bucket_t));
    if(db->table == NULL){
        free(db);
        return NULL;
    }
    memset(db->table, 0, db->capacity * sizeof(bucket_t));
    if(add_new_db(&db_list_head, db) == -1){
        return NULL;
    }
    return db;
}


db_t *
open_db(char *name){
    return find_db(&db_list_head, name);
}

int
close_db(db_t *db){
    return delete_db(&db_list_head, db);
}

int
db_run_cmd(db_data_t *in_data, db_data_t *out_data)
{
    return 0;
}