#include <stdio.h>

#include "tr_data.h"
#include "resources.h"
#include "net.h"
#include "traceroute.h"
#include<signal.h>  // for handling signal 

#define EXIT_SIGN_TERM 130

void handle_sigint(int signal) 
{
    if (signal == SIGINT) {
        free_resources();
        exit(EXIT_SIGN_TERM);
    }
}

static void print_man() {
    printf("Usage: %s --help | HOST\n", g_tr_data.executable_name);
    printf("%s - print the route packets trace to network host.\n", g_tr_data.executable_name);
    printf("Options:\n");
    printf("--help give this help list\n");
}

int main(int ac, char **av)
{
    signal(SIGINT,handle_sigint);
    
    init_tr_data();
    parse_args(ac, av);

    if (g_tr_data.print_man_only)
    {
        print_man();
        exit(EXIT_SUCCESS);
    }

    init_resouces();
    
    get_addr_info(g_tr_data.target_host_arg, &g_resources.target_addr_info);

    sockaddr_to_ipv4_str(g_resources.target_addr_info->ai_addr, g_tr_data.target_ipv4_str);

    open_send_socket();
    open_receive_socket();
    set_recv_timeout(g_resources.receive_socket, &g_tr_data.probe_timeout);

    traceroute();

    free_resources();
    exit(EXIT_SUCCESS);
}

