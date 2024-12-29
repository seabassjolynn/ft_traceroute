#include "traceroute.h"

static float calc_round_trip_time_ms(struct timeval *start, struct  timeval *end)
{
    return (float)((end->tv_sec - start->tv_sec) * 1000000 + (end->tv_usec - start->tv_usec)) / 1000.0;
}

static bool is_tr_icmp(struct s_icmp_error_frame *frame)
{
    return (frame->original_data.ip_header.protocol == IPPROTO_UDP) && 
    (
        ((frame->type == ICMP_TYPE_TIME_EXCEEDED && frame->code) == (ICMP_CODE_TIME_EXCEEDED_IN_TRANSIT)) ||
        ((frame->type == ICMP_TYPE_DESTINATION_UNREACHABLE) && (frame->code == ICMP_CODE_PORT_UNREACHABLE))
    );
}

static struct s_probe probe(struct in_addr local_ip, int hop_num, uint16_t dst_prot)
{
    set_ttl(g_resources.send_socket, hop_num);
    
    ((struct sockaddr_in*) g_resources.target_addr_info->ai_addr)->sin_port = htons(dst_prot);
    
    struct timeval request_time = current_time();
    
    uint32_t timestamp = request_time.tv_sec * 1000000 + request_time.tv_usec;

    uint32_t src_addr = local_ip.s_addr;
    uint32_t dst_addr = ((struct sockaddr_in*)g_resources.target_addr_info->ai_addr)->sin_addr.s_addr;
    
    uint16_t src_port = 33434;
    struct iphdr ip_header = create_ip_header(hop_num, src_addr, dst_addr, sizeof(struct s_udp_frame));
    
    struct s_udp_frame udp_frame = create_udp_frame(&ip_header, src_port, dst_prot, timestamp);
    
    struct s_ip_frame ip_frame;
    ip_frame.header = ip_header;
    ip_frame.body = udp_frame;

    int result = sendto(g_resources.send_socket, &ip_frame, sizeof(ip_frame), 0, (g_resources.target_addr_info)->ai_addr, (g_resources.target_addr_info)->ai_addrlen);
    if (result == -1)
    {
        printf("Error when sending UDP packet: %s\n", strerror(errno));
        free_resources();
        exit(EXIT_FAILURE);
    }
    
    uint8_t received_ip_packet[IP_PACKET_BUFFER_LENGTH];
    ft_memset(received_ip_packet, 0, IP_PACKET_BUFFER_LENGTH);
    
    struct s_probe probe;
    probe.reached_destination = false;
    probe.round_trip_time = -1;
    
    while ((result = recvfrom(g_resources.receive_socket, received_ip_packet, IP_PACKET_BUFFER_LENGTH, 0, NULL, NULL)) > 0)
    {
        DEBUG_LOG("Received something %d\n", result);
        
        struct s_icmp_error_frame *received_icmp_frame = (struct s_icmp_error_frame *) (received_ip_packet + IP_HEADER_LENGTH);
        if (is_tr_icmp(received_icmp_frame))
        {
            DEBUG_LOG("Sent checksum: %d\n", udp_frame.header.checksum);
            DEBUG_LOG("Received checksum: %d\n", received_icmp_frame->original_data.udp_frame.header.checksum);
            if (received_icmp_frame->original_data.udp_frame.header.checksum == udp_frame.header.checksum)
            {
                get_src_addr(received_ip_packet, probe.remote_addr_str);               
                struct timeval reply_time = current_time();
                probe.round_trip_time = calc_round_trip_time_ms(&request_time, &reply_time);
                probe.reached_destination = received_icmp_frame->type == ICMP_TYPE_DESTINATION_UNREACHABLE && received_icmp_frame->code == ICMP_CODE_PORT_UNREACHABLE;
                return probe;
            }
            else
            {
                DEBUG_LOG("Didn't match body\n");
            }
        }
        else
        {
            DEBUG_LOG("Not error reply");
        }
    }
    return probe;
}

static void print_traceroute_header()
{
    printf("traceroute to %s (%s), %d hops max\n", g_tr_data.target_host_arg, g_tr_data.target_ipv4_str, MAX_HOPS);
}

static void print_hop_num(int hop_num)
{
    printf("%3d   ", hop_num);
}

static void print_probe(int probe_num_in_hop, struct s_probe *prev_succes_probe, struct s_probe *current_probe)
{        
    if (probe_num_in_hop == 0)
    {
        if (current_probe->round_trip_time != -1)
        {
            printf("%s  %.3fms", current_probe->remote_addr_str, current_probe->round_trip_time);    
        }
        else
        {
            printf("*");
        }
    }
    else
    {
        if (prev_succes_probe->round_trip_time != -1)
        {
            if (current_probe->round_trip_time != -1)
            {
                if (ft_strncmp(prev_succes_probe->remote_addr_str, current_probe->remote_addr_str, INET6_ADDRSTRLEN) == 0)
                {
                    printf("  %.3fms", current_probe->round_trip_time);
                }
                else
                {
                    printf("  %s  %.3fms", current_probe->remote_addr_str, current_probe->round_trip_time);
                }
            }
            else
            {
                printf("  *");
            }
        }
        else
        {
            if (current_probe->round_trip_time != -1)
            {
                printf("  %s  %.3fms", current_probe->remote_addr_str, current_probe->round_trip_time);    
            }
            else
            {
                printf("  *");
            }
        }
    }

    if (probe_num_in_hop == PROBES_PER_HOP_NUM - 1)
    {
        printf("\n");
    }
}

void traceroute()
{
    print_traceroute_header();
    
    int hop_num = 1;
    int port = START_PORT;
    struct in_addr local_ip = get_local_ip();
    
    bool reached_destination = false;
    while (!reached_destination)
    {
        print_hop_num(hop_num);
        
        int probe_num = 0;
        
        struct s_probe prev_probe;
        prev_probe.round_trip_time = -1;
        
        struct s_probe current_probe;
        current_probe.round_trip_time = -1;
        
        while(probe_num < PROBES_PER_HOP_NUM)
        {   
            current_probe = probe(local_ip, hop_num, port);
            
            print_probe(probe_num, &prev_probe, &current_probe);
            if (current_probe.round_trip_time != -1)
            {
                prev_probe = current_probe;
            }
            probe_num++;
        }
        reached_destination = current_probe.reached_destination;
        port++;
        hop_num++;
    }
}