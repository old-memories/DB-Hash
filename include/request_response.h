#ifndef REQUEST_RESPONSE_H
#define REQUEST_RESPONSE_H

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

#endif