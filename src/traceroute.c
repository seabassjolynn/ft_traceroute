#include "traceroute.h"

static void print_traceroute_header()
{
    printf("traceroute to %s (%s), %d hops max\n", g_tr_data.target_host_arg, g_tr_data.target_addr, MAX_HOPS);
}

void traceroute()
{
    print_traceroute_header();
    int i = 1;
    while (i <= MAX_HOPS)
    {
        int j = 0;
        while(j < PROBES_PER_HOP_NUM)
        {
            
        }
    }
}