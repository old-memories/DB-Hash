
#include "client.h"
#include "my_db.h"

extern int errno;

static void
create_req(request_t *req, char *input)
{
    /*
        put [key] [val]
        set [key] [val]
        del [key] [val]
    */
}

static void
show_rsp(response_t *rsp)
{
    
}

int
run_client(conf_t *conf)
{
    int sock_server;
    int child_pid;
    int pipe_fd[2];
    int epfd;
    struct sockaddr_in server_addr;
    


    static struct epoll_event ev, events[2];
    ev.events = EPOLLIN | EPOLLET;

    int stop = 0;

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


    if(pipe(pipe_fd) < 0){
        fprintf(stderr, "pipe failed\n");
        return -1;
    }


    if((epfd = epoll_create(CLIENT_EPOLL_SIZE)) < 0){
        fprintf(stderr, "epoll_create failed\n");
        return -1;
    }

    ev.data.fd = sock_server;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, sock_server, &ev) < 0){
        fprintf(stderr, "epoll_ctl failed\n");
        return -1;
    }

    ev.data.fd = pipe_fd[0];
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, pipe_fd[0], &ev) < 0){
        fprintf(stderr, "epoll_ctl failed\n");
        return -1;
    }
    if((child_pid = fork()) < 0){
        fprintf(stderr, "fork failed\n");
        return -1;
    }
    switch (child_pid) {
    case 0:        
        close(pipe_fd[0]);
        close(sock_server);
        fprintf(stdout, "enter \"%s\" to exit the program\n", CLIENT_EXIT_STRING);    
        while (!stop) {
            fprintf(stdout, "%s:%lu>", conf->addr, conf->port);
            char input[CLIENT_INPUT_SIZE];
            memset(input, 0, CLIENT_INPUT_SIZE);
            fgets(input, CLIENT_INPUT_SIZE, stdin);

            if (strncasecmp(input, CLIENT_EXIT_STRING, strlen(CLIENT_EXIT_STRING)) == 0) {
            fprintf(stdout, "EXIT. close pipe\n");
                close(pipe_fd[1]);
                stop = 1;
                break;
            } else {
                /*
                    strlen(message) - 1 : Throw the last newline char
                */
                if(write(pipe_fd[1], input, strlen(input) - 1) < 0){
                    fprintf(stderr, "write failed. close pipe\n");
                    close(pipe_fd[1]);
                    stop = 1;
                    break;
                }
            }
        }
        break;
    default:        
        close(pipe_fd[1]);    
        int epoll_events_count, res;
        while (!stop) {
            if((epoll_events_count = epoll_wait(epfd, events, CLIENT_EPOLL_SIZE, -1)) < 0){
                close(pipe_fd[0]);
                close(sock_server);
                stop = 1;
                break;            
            }
            for (int i = 0; i < epoll_events_count; i++) {
                if (events[i].data.fd == sock_server) {
                    char recv_buf[CLIENT_RECV_BUF_SIZE];
                    memset(recv_buf, 0, CLIENT_RECV_BUF_SIZE);
                    int len = 0;

                    if((len = recv(sock_server, recv_buf, CLIENT_RECV_BUF_SIZE, 0)) <= 0){
                        fprintf(stderr, "recv failed. close pipe and sock_server\n");
                        close(pipe_fd[0]);
                        close(sock_server);
                        stop = 1;
                        break;
                    }

                    response_t rsp;
                    memset(&rsp, 0, sizeof(response_t));
                    memcpy(&rsp, recv_buf, sizeof(response_t));
                    show_rsp(&rsp);                    
                    
                } else {
                    char read_buf[CLIENT_INPUT_SIZE];
                    memset(read_buf, 0, CLIENT_INPUT_SIZE);
                    int len = 0;

                    if((len = read(events[i].data.fd, read_buf, CLIENT_INPUT_SIZE)) <= 0){
                        if(len == 0){
                            fprintf(stderr, "EXIT. close pipe and sock_server\n");
                        }
                        else{
                            fprintf(stderr, "read failed. close pipe and sock_server\n");
                        }
                        close(pipe_fd[0]);
                        close(sock_server);
                        stop = 1;
                        break;
                    }

                    request_t req;
                    memset(&req, 0, sizeof(request_t));
                    create_req(&req, read_buf);

                    char send_buf[CLIENT_SEND_BUF_SIZE];
                    memset(send_buf, 0, CLIENT_SEND_BUF_SIZE);
                    memcpy(send_buf, &req, sizeof(request_t));

                    if(send(sock_server, send_buf, CLIENT_SEND_BUF_SIZE, 0) < 0){
                        fprintf(stderr, "send failed. close pipe and sock_server\n");
                        close(pipe_fd[0]);
                        close(sock_server);
                        stop = 1;
                        break;
                    }
                }
            }
        }
    }

    if(child_pid){
        waitpid(child_pid, NULL, 0);
    }

    return 0;

}
