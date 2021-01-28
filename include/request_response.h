#ifndef REQUEST_RESPONSE_H
#define REQUEST_RESPONSE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "my_db.h"

typedef enum rsp_status {
    RSP_SUCCESS,
    RSP_FAILURE,
} rsp_status_t;

typedef struct request {
    db_data_t data;
    
} request_t;

typedef struct response {
    db_data_t data;
    rsp_status_t status;
} response_t;

extern int create_req(request_t *req, char *input);
extern int create_rsp(request_t *req, response_t *rsp);
extern void show_rsp(FILE *stream,  response_t *rsp);

#endif