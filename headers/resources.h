#ifndef RESOURCES_H
#define RESOURCES_H

//addrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct s_resources
{
    int send_socket;
    int receive_socket;
    struct addrinfo *target_addr_info;
};

extern struct s_resources g_resources;

void free_resources();

void init_resouces();

#endif