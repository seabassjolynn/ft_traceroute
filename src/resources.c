#include "resources.h"
#include <stdlib.h>

#include <unistd.h>

#include "exit.h"

struct s_resources g_resources;

void init_resouces()
{
    g_resources.send_socket = -1;
    g_resources.receive_socket = -1;
    g_resources.target_addr_info = NULL;
}

void free_resources()
{
    close(g_resources.send_socket);
    close(g_resources.receive_socket);
    freeaddrinfo(g_resources.target_addr_info);
}