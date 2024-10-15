#include <stdio.h>

#include "tr_data.h"
#include "exit.h"
#include "resources.h"
#include "net.h"
#include "traceroute.h"

static void print_man() {
    printf("Usage: %s --help | HOST\n", g_tr_data.executable_name);
    printf("%s - print the route packets trace to network host.\n", g_tr_data.executable_name);
    printf("Options:\n");
    printf("--help give this help list\n");
}

int main(int ac, char **av)
{
    set_defaults();
    parse_args(ac, av);

    if (g_tr_data.print_man_only)
    {
        print_man();
        exit(SUCCESS);
    }

    init_resouces();
    
    get_addr_info(g_tr_data.target_host_arg, &g_resources.target_addr_info);

    sockaddr_to_ipv4_addr_str(g_resources.target_addr_info->ai_addr, g_tr_data.target_addr);

    open_send_socket();
    confugure_send_scocket();
    
    open_receive_socket();
    confugure_receive_scocket();

    traceroute();

    free_resources();
    exit(SUCCESS);
}