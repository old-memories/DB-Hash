#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "conf.h"
#include "request_response.h"

#define CLIENT_INPUT_SIZE 1 << 12
#define CLIENT_EXIT_STRING "EXIT"

extern int run_client(conf_t *conf);

#endif