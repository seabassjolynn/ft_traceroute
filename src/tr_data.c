#include "tr_data.h"

struct s_tr_data g_tr_data;

void init_tr_data()
{
    struct timeval timeval;
    timeval.tv_sec = 2;
    timeval.tv_usec = 0;
    
    g_tr_data.probe_timeout = timeval; 
    g_tr_data.print_man_only = false;
    g_tr_data.executable_name = NULL;
    g_tr_data.target_host_arg = NULL;
}