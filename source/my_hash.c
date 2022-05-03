#include <stdio.h>
#include <stdlib.h>

#include "client.h"
#include "server.h"
#include "conf.h"




int
main(int argc, char *argv[])
{
    conf_t conf;

    int ret = 0;

    parse_conf(argc, argv, &conf);

    if(conf.server){
        printf("server side\n");
        ret = run_server(&conf);   
    }
    else{
        ret = run_client(&conf);
    }

    printf("exited main program, return [%d]\n", ret);
    return ret;
}