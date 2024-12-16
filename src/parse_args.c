#include <stdio.h>
#include <string.h>
#include <stdlib.h> //EXIT_FAILURE const
#include "tr_data.h"
#include "utils.h"

void parse_args(int ac, char *av[]) {
    g_tr_data.executable_name = av[0];
    
    if (ac != 2)
    {
        printf("%s requires only one argument: host or --help\n", g_tr_data.executable_name);
        exit(EXIT_FAILURE);
    }
    
    if (av[1][0] == '-')
    {
        if (ft_strncmp(av[1], "--help", ft_strlen("--help") + 1) == 0)
        {
            g_tr_data.print_man_only = true;
            return;
        }
        else
        {
            printf("%s: unkown argument %s\n", g_tr_data.executable_name, av[1]);    
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        g_tr_data.target_host_arg = av[1];
    }
}