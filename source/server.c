
#include "server.h"
#include "my_db.h"


static void *
run_cmd(void *arg)
{
    int sock_client = *((int *)arg);
    char recv_buf[SERVER_RECV_BUF_SIZE];
    memset(recv_buf, 0, SERVER_RECV_BUF_SIZE);
    int len = 0;

    if((len = recv(sock_client, recv_buf, SERVER_RECV_BUF_SIZE, 0)) <= 0){
        if(len == 0){
            fprintf(stdout, "client: [%d] exited\n", sock_client);
        }
        else {
            fprintf(stdout, "recv failed. close client: [%d]\n", sock_client);
        }
        close(sock_client);
        return NULL;
    }

    request_t req;
    memset(&req, 0, sizeof(request_t));
    memcpy(&req, recv_buf, sizeof(request_t));

    response_t rsp;
    memset(&rsp, 0, sizeof(request_t));
    
    create_rsp(&req, &rsp);

    char send_buf[SERVER_SEND_BUF_SIZE];
    memset(send_buf, 0, SERVER_SEND_BUF_SIZE);
    memcpy(send_buf, &rsp, sizeof(response_t));
    if(send(sock_client, send_buf, SERVER_SEND_BUF_SIZE, 0) < 0){
        fprintf(stdout, "send failed. close client: [%d]\n", sock_client);
        close(sock_client);
        return NULL;
    }
    return NULL;

}

static int
set_nonblock_socket(int sockfd)
{
    return fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
}

int
run_server(conf_t *conf)
{    
    db_t *db = NULL;
    int sock_listener, epfd, sock_client, epoll_events_count;
    struct sockaddr_in server_addr, client_addr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    static struct epoll_event ev, events[SERVER_EPOLL_SIZE];


    if(conf->create){
        if((db = create_db(conf->name, conf->db_size)) == NULL){
            fprintf(stderr, "create new db failed\n");
            return -1;
        }
    }
    if((db = open_db(conf->name)) == NULL){
        fprintf(stderr, "open db failed\n");
        return -1;
    }

    if((sock_listener = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "socket failed\n");
        return -1;

    }
    if(set_nonblock_socket(sock_listener) < 0){
        fprintf(stderr, "set_nonblock_socket failed\n");
        return -1;

    }

    int on = 1;
    if((setsockopt(sock_listener, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0){
        fprintf(stderr, "setsockopt failed\n");
        return -1;

    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(conf->addr);
    server_addr.sin_port = htons(conf->port);

    if(bind(sock_listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        fprintf(stderr, "bind failed\n");
        return -1;

    }
    if(listen(sock_listener, SERVER_LISTEN_NUM) < 0){
        fprintf(stderr, "listen failed\n");

        return -1;

    }

    if((epfd = epoll_create(SERVER_EPOLL_SIZE)) < 0){
        fprintf(stderr, "epoll_create failed\n");
        return -1;

    }
    ev.data.fd = sock_listener;
    ev.events = EPOLLIN | EPOLLET;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, sock_listener, &ev) < 0){
        fprintf(stderr, "epoll_ctl failed\n");
        return -1;

    }

    fprintf(stdout, "server socket: [%d] and epfd: [%d] created\n", sock_listener, epfd);

    while(1){
        if((epoll_events_count = epoll_wait(epfd, events, SERVER_EPOLL_SIZE, -1)) < 0){
        fprintf(stderr, "epoll_wait failed\n");
            return -1;
        }
        for(int i = 0; i < epoll_events_count; i++){
            if(events[i].data.fd == sock_listener){
                if((sock_client = accept(sock_listener, (struct sockaddr *)&client_addr, &socklen)) < 0){
                    fprintf(stderr, "accept failed\n");
                    return -1;
                }
                if(set_nonblock_socket(sock_client) < 0){
                    fprintf(stderr, "set_nonblock_socket failed\n");
                    return -1;

                }                
                ev.data.fd = sock_client;
                if(epoll_ctl(epfd, EPOLL_CTL_ADD, sock_client, &ev) < 0){
                    fprintf(stderr, "epoll_ctl failed\n");
                    return -1;
                }
                fprintf(stdout, "accept new client: [%d]\n", sock_client);
                /* TODO: Record sock_client in a set.

                */
            }
            else{
                fprintf(stdout, "create a new thread to run command for client: [%d]\n", events[i].data.fd);
                pthread_t cmd_handler;
                int arg = events[i].data.fd;
                if(pthread_create(&cmd_handler, NULL, run_cmd, (void *)&arg) < 0){
                    return -1;
                }
            }

        }
    }
    return 0;
}