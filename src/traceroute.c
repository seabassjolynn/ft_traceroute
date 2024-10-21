#include "traceroute.h"

static float calc_round_trip_time_ms(struct timeval *start, struct  timeval *end)
{
    return (float)((end->tv_sec - start->tv_sec) * 1000000 + (end->tv_usec - start->tv_usec)) / 1000.0;
}

static struct s_probe probe(int hop_num, uint16_t dst_prot)
{
    set_ttl(g_resources.send_socket, hop_num);
    
    ((struct sockaddr_in*) g_resources.target_addr_info->ai_addr)->sin_port = htons(dst_prot);
    
    
    struct timeval request_time = current_time();
    
    uint32_t timestamp = request_time.tv_sec * 1000000 + request_time.tv_usec;

    uint32_t src_addr = inet_addr("192.168.43.57");
    uint32_t dst_addr = inet_addr("142.251.36.238");
    
    uint16_t src_port = 33434;
    struct iphdr ip_header = create_ip_header(hop_num, src_addr, dst_addr, sizeof(struct s_udp_frame));
    
    struct s_udp_frame udp_frame = create_udp_frame(&ip_header, src_port, dst_prot, timestamp);
    
    struct s_ip_frame ip_frame;
    ip_frame.header = ip_header;
    ip_frame.body = udp_frame;

    int result = sendto(g_resources.send_socket, &ip_frame, sizeof(ip_frame), 0, (g_resources.target_addr_info)->ai_addr, (g_resources.target_addr_info)->ai_addrlen);
    if (result == -1)
    {
        fprintf(stderr, "Error when sending UDP packet: %s\n", strerror(errno));
        free_resources();
        exit(EXIT_FAILURE);
    }
    
    uint8_t received_ip_packet[IP_PACKET_BUFFER_LENGTH];
    ft_memset(received_ip_packet, 0, IP_PACKET_BUFFER_LENGTH);
    
    struct s_probe probe;
    probe.round_trip_time = -1;
    result = recvfrom(g_resources.receive_socket, received_ip_packet, IP_PACKET_BUFFER_LENGTH, 0, NULL, NULL);
    printf("%s\n", strerror(errno));
    while ((result = recvfrom(g_resources.receive_socket, received_ip_packet, IP_PACKET_BUFFER_LENGTH, 0, NULL, NULL)) > 0)
    {
        printf("Received something %d\n", result);
        struct s_icmp_error_frame *received_icmp_frame = (struct s_icmp_error_frame *) (received_ip_packet + IP_HEADER_LENGTH);
        if (received_icmp_frame->original_data.ip_header.protocol == IPPROTO_UDP && received_icmp_frame->type == ICMP_TIME_EXCEEDED_TYPE && received_icmp_frame->code == ICMP_TIME_EXCEEDED_IN_TRANSIT_CODE)
        {
            if (received_icmp_frame->original_data.udp_frame.data == timestamp)
            {
                struct s_probe probe;
                get_src_addr(received_ip_packet, probe.remote_addr_str);               
                struct timeval reply_time = current_time();
                probe.round_trip_time = calc_round_trip_time_ms(&request_time, &reply_time);
                return probe;
            }
            else
            {
                printf("Didn't match body\n");
            }
        }
        else
        {
            printf("Not error reply");
        }
    }
    return probe;
}

static void print_traceroute_header()
{
    printf("traceroute to %s (%s), %d hops max\n", g_tr_data.target_host_arg, g_tr_data.target_addr, MAX_HOPS);
}

void traceroute()
{
    print_traceroute_header();
    
    int hop_num = 1;
    int port = START_PORT;
    
    while (hop_num <= MAX_HOPS)
    {
        int probe_num = 0;
        while(probe_num < PROBES_PER_HOP_NUM)
        {
            struct s_probe p = probe(hop_num, port);
            if (p.round_trip_time > 0)
            {
                printf("Host: %s time: %f\n", p.remote_addr_str, p.round_trip_time);
            }
            else
            {
                printf("Time out\n");
            }
            probe_num++;
        }
        port++;
        hop_num++;
    }
}