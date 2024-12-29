#include "resources.h"
#include <stdlib.h>

#include <unistd.h>

struct s_resources g_resources;

void init_resouces()
{
    g_resources.send_socket = -1;
    g_resources.receive_socket = -1;
    g_resources.target_addr_info = NULL;
}

void free_resources()
{
    if (g_resources.send_socket != -1)
    {
        close(g_resources.send_socket);
    }
    
    if (g_resources.receive_socket != -1) 
    {
        close(g_resources.receive_socket);
    }
    
    if (g_resources.target_addr_info != NULL) 
    {
        freeaddrinfo(g_resources.target_addr_info);
    }
}