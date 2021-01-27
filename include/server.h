#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include "conf.h"
#include "request_response.h"

#define SERVER_EPOLL_SIZE 1024
#define SERVER_LISTEN_NUM 1
#define SERVER_RECV_BUF_SIZE 1024
#define SERVER_SEND_BUF_SIZE 65536


extern int run_server(conf_t *conf);

#endif