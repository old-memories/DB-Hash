
#include "client.h"

extern int errno;

int
run_client(conf_t *conf)
{
    int sock_server;
    struct sockaddr_in server_addr;
    request_t req;
    response_t rsp;
    char input[CLIENT_INPUT_SIZE];
    int recv_len;

    if((sock_server = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "socket failed. errno: [%d]\n", errno);
        return -1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(conf->port);
    server_addr.sin_addr.s_addr = inet_addr(conf->addr);
    
    if(connect(sock_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        fprintf(stderr, "connect failed. errno: [%d]\n", errno);
        return -1;
    }
    
    fprintf(stdout, "enter \"%s\" to exit the program\n", CLIENT_EXIT_STRING);    
    
    while (1) {
        fprintf(stdout, "%s:%lu>", conf->addr, conf->port);
        memset(input, 0, CLIENT_INPUT_SIZE);
        fgets(input, CLIENT_INPUT_SIZE, stdin);
        /* remove trailing newline:*/
        input[strcspn(input, "\n")] = '\0';

        if (strncasecmp(input, CLIENT_EXIT_STRING, strlen(CLIENT_EXIT_STRING)) == 0) {
            fprintf(stdout, "EXIT.\n");
            goto out;
            break;
        } 
        
        memset(&req, 0, sizeof(request_t));
        if(create_req(&req, input) != 0){
            fprintf(stdout, "create_req failed. request format error\n");
            goto out;
        }

        if(send(sock_server, (void *)&req, sizeof(request_t), 0) < 0){
            fprintf(stderr, "send failed. close sock_server\n");
            goto out;
        }


        memset(&rsp, 0, sizeof(response_t));
        if((recv_len = recv(sock_server, (void *)&rsp, sizeof(response_t), 0)) <= 0){
            if(recv_len == 0) {
                fprintf(stderr, "server exited. close sock_server\n");
            } else {
                fprintf(stderr, "recv failed. close pipe and sock_server\n");
            }
            goto out;
        }
        fprintf(stdout, "\n");
        show_rsp(stdout, &rsp);    
    
    }

out:
    close(sock_server);
    return 0;

}
