#ifndef NET_H
#define NET_H

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "utils.h"
#include "resources.h"
#include "tr_data.h"
#include <errno.h> //errno
#include <string.h> //strerror
#include <stdlib.h> //EXIT_FAILURE / EXIT_SUCCESS consts

#include <netinet/ip.h> //ip header

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include "utils.h"

#define IP_PACKET_BUFFER_LENGTH 1024
#define IP_HEADER_LENGTH 20
#define ICMP_TYPE_TIME_EXCEEDED 11
#define ICMP_CODE_TIME_EXCEEDED_IN_TRANSIT 0
#define ICMP_TYPE_DESTINATION_UNREACHABLE 3
#define ICMP_CODE_PORT_UNREACHABLE 3
#define ECHO_REQUEST_TYPE 8

struct s_udp_header
{
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t total_length;
    uint16_t checksum;
};

struct s_udp_frame
{
    struct s_udp_header header;
    uint32_t data;
};

struct s_original_data
{
    struct iphdr ip_header;
    struct s_udp_frame udp_frame;
};

struct s_icmp_error_frame
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint32_t unused;
    struct s_original_data original_data;
};

struct s_ip_frame
{
    struct iphdr header;
    struct s_udp_frame body;
};

struct s_ping_header
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence_number;
};

struct s_icmp_echo_frame
{
    struct s_ping_header header;
    unsigned char payload[10];
};

void get_addr_info(char *host, struct addrinfo **addr_info);

void open_send_socket();

void open_receive_socket();

void set_ttl(int socket, int ttl);

void set_recv_timeout(int socket, struct timeval *timeout);

void sockaddr_to_ipv4_addr_str(struct sockaddr *sockaddr, char *addr);

struct s_udp_frame create_udp_frame(struct iphdr *iphdr, uint16_t src_port, uint16_t dst_port, uint32_t data);

void get_src_addr(void *ip_packet, char *ipv4_addr_str);

struct timeval current_time();

struct s_ip_frame create_ip_frame(uint8_t ttl, uint32_t dst_ip, struct s_udp_frame body);

struct iphdr create_ip_header(uint8_t ttl, uint32_t src_ip_net, uint32_t dst_ip_net, uint16_t payload_len);

struct s_icmp_echo_frame create_echo_request();

uint32_t get_local_ip_net_byte_order();

#endif