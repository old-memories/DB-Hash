#include "request_response.h"


/*
    get [key]
    set [key] [val]
    del [key]
*/
int 
create_req(request_t *req, char *input)
{

    char * pos[3];
    char * pch;
    int i = 0;
    pch = strtok(input, " ");
    while(pch != NULL && i < 3){
        pos[i++] = pch;
        // fprintf(stdout, "strtok: %s\n", pch);
        pch = strtok(NULL," ");
    }
    if(i < 2){
        return -1;
    }
    i = 0;
    if(strcmp("get", pos[i]) == 0){
        i++;
        req->data.cmd_type = GET;
        req->data.key_len = strlen(pos[i]);
        req->data.key = malloc(req->data.key_len);
        memcpy(req->data.key, pos[i], req->data.key_len);
        return 0;
    }
    if(strcmp("del", pos[i]) == 0){
        i++;
        req->data.cmd_type = DELETE;
        req->data.key_len = strlen(pos[i]);
        req->data.key = malloc(req->data.key_len);
        memcpy(req->data.key, pos[i], req->data.key_len);
        return 0;
    }    
    if(strcmp("set", pos[i]) == 0){
        i++;
        req->data.cmd_type = SET;
        req->data.key_len = strlen(pos[i]);
        req->data.key = malloc(req->data.key_len);
        memcpy(req->data.key, pos[i], req->data.key_len);
        i++;
        req->data.value_len = strlen(pos[i]);
        req->data.value = malloc(req->data.value_len);
        memcpy(req->data.value, pos[i], req->data.value_len);
        return 0;
    }
    
    return -1;


}

int
create_rsp(db_t *db, request_t *req, response_t *rsp)
{

   int ret = 0;
    /*
        db_run_cmd
    */
   memcpy(&(rsp->data), &(req->data), sizeof(db_data_t));
   ret = db_run_cmd(db, &(rsp->data));
   if(ret == 0){
       rsp->status = RSP_SUCCESS;
   }
   else{
       rsp->status = RSP_FAILURE;
   }
   return 0;
   
}

void
show_rsp(FILE *stream,  response_t *rsp)
{
    fprintf(stream, "status: [%s]\n", rsp->status == RSP_SUCCESS ? "RSP_SUCCESS" : "RSP_FAILURE");
    fprintf(stream, "command: [%d]\n", (int)(rsp->data.cmd_type));
    fprintf(stream, "command status: [%d]\n", (int)(rsp->data.status));
    fprintf(stream, "key: [%.*s]\n", (int)(rsp->data.key_len), (char *)(rsp->data.key));
    if(rsp->data.value_len != 0){
        fprintf(stream, "value: [%.*s]\n", (int)(rsp->data.value_len), (char *)(rsp->data.value));
    }


}