#ifndef TR_DATA_H
#define TR_DATA_H

#include <stdbool.h>
#include <sys/time.h> //timeval
#include <arpa/inet.h> //INET_ADDRLEN define
#include <stdio.h> //NULL macro

extern struct s_tr_data g_tr_data;

void set_defaults(void);

void parse_args(int ac, char *av[]);

struct s_tr_data
{
    char *executable_name;
    char *target_host_arg;
    char target_addr[INET_ADDRSTRLEN];
    struct timeval probe_timeout;
    bool print_man_only;
};

#endif