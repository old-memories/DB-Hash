#include <stdio.h>
#include"conf.h"

static const conf_t conf_default = {
    .name = "test",
    .server = 1,
    .addr = "127.0.0.1",
    .port = 8081,
    .db_size = 1 << 10,
    .create = 1
};

static void
print_conf(conf_t *conf){
    (void) fprintf(stdout,
        "conf:\n"
        "\tname: [%s]\n"
        "\tserver: [%d]\n"
        "\taddr: [%s]\n"
        "\tport: [%lu]\n"
        "\tdb_size: [%lu]\n"
        "\tcreate: [%d]\n",
        conf->name,
        conf->server,
        conf->addr,
        conf->port,
        conf->db_size,
        conf->create
    );
}

static void
usage()
{
    (void) fprintf(stdout,
        "Usage:\n"
        "\t-n [name] name of the db to be opened\n"
        "\t-r [1 or 0] server side or client side\n"
        "\t-c [1 or 0] server should create the db or not\n"
        "\t-a ip address of server\n"
        "\t-p port of server\n"
        "\t-s size of the db(only valid when creating)\n"

    );
}

void
parse_conf(int argc, char **argv, conf_t *conf)
{
    const char short_opts[] = "n:a:p:s:r:c:h";

    int opt;
    
    memmove(conf, &conf_default, sizeof(conf_t));

    while((opt = getopt(argc, argv, short_opts)) != -1){

        switch (opt){
            case 'n':
                snprintf(conf->name, NAME_LEN, "%s", optarg);
                break;
            case 'a':
                snprintf(conf->addr, ADDR_LEN, "%s", optarg);
                break;
            case 'p':
                conf->port = strtol(optarg, NULL, 0);
                break;
            case 's':
                conf->db_size = strtol(optarg, NULL, 0);
                break;
            case 'r':
                conf->server = !!strtol(optarg, NULL, 0);
                break; 
            case 'c':
                conf->create = !!strtol(optarg, NULL, 0);
                break; 
            case 'h':
            case '?':
            default :
                usage();
                exit(0);
        }
    }
    print_conf(conf);
}