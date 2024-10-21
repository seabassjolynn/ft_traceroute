#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include "tr_data.h"
#include "net.h"

#define MAX_HOPS 3

#define PROBES_PER_HOP_NUM 3

#define START_PORT 33434

void traceroute();

struct s_probe
{
    char remote_addr_str[INET_ADDRSTRLEN];
    float round_trip_time;
};

#endif