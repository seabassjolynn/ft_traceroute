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

#define IP_PACKET_BUFFER_LENGTH 1024
#define IP_HEADER_LENGTH 20
#define ICMP_TYPE_TIME_EXCEEDED 11
#define ICMP_CODE_TIME_EXCEEDED_IN_TRANSIT 0
#define ICMP_TYPE_DESTINATION_UNREACHABLE 3
#define ICMP_CODE_PORT_UNREACHABLE 3

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

struct s_udp_frame create_udp_frame_1(uint32_t src_addr, uint32_t dst_addr, uint16_t src_port, uint16_t dst_port, uint32_t data);

void get_src_addr(void *ip_packet, char *ipv4_addr_str);

struct timeval current_time();

struct s_ip_frame create_ip_frame(uint8_t ttl, uint32_t dst_ip, struct s_udp_frame body);

struct iphdr create_ip_header(uint8_t ttl, uint32_t src_ip_net, uint32_t dst_ip_net, uint16_t payload_len);

struct s_icmp_echo_frame create_echo_request();

uint32_t get_local_ip();


// #define SRC_ADDR_OFFSET_IN_IP_PACKET 12

// int get_ip_header_length_in_bytes(uint8_t *packet);

// uint16_t caclulate_checksum(void *addr, int len);

// #define ECHO_PACKET_BODY_LENGTH 56 //this number is provided with -s flag. This is body length of echo packet. And this is what is written in ping header. 
// //ECHO request length is then 56 + 8 (header length). String that describes received string writes number of bytes. And this number is length of ping reply packet 
// //which is equal to length of echo request packet, and not IP packat
// #define ECHO_PACKET_HEADER_LENGTH 8
// #define ECHO_PACKET_LENGTH ECHO_PACKET_BODY_LENGTH + ECHO_PACKET_HEADER_LENGTH
// // 60 bytes - maximum IP header length
// // ICMP: Destination Unreachable Message - length is 8 (header) + ip header + 64 bits of packet data = 8 + 60 + 8 = 76 byte.
// // the other types of ICMP messages are shorter

#define ECHO_REQUEST_TYPE 8

// #define ECHO_REQUEST_CODE 0

// #define ECHO_REPLY_TYPE 0

// #define ECHO_REPLY_CODE 0

// #define ORIGINAL_DATAGRAM_FRAGMENT_LENGTH_IN_ERROR_REPLY 64

// #define TIME_OUT -1

// # include <stdint.h>
// # include <stdbool.h>



// struct s_icmp_echo_packet
// {
//     struct s_ping_header header;
//     unsigned char payload[ECHO_PACKET_BODY_LENGTH];
// };

// struct s_icmp_echo_packet create_echo_request();

// struct s_ip_header
// {
//     uint8_t version_and_ihl;
//     uint8_t tos;
//     uint16_t total_length;
//     uint16_t identification;
//     uint16_t flags_and_fragment_offset;
//     uint8_t ttl;
//     uint8_t protocol;
//     uint16_t header_checksum;
//     uint32_t src_address;
//     uint32_t dst_address;
// };

// struct s_original_data
// {
//     struct s_ip_header ip_header;
//     struct s_ping_header ping_header;
// };

// struct s_error_ping_reply
// {
//     uint8_t type;
//     uint8_t code;
//     uint16_t checksum;
//     uint32_t unused;
//     struct s_original_data original_data;
// };

// bool is_ping_error_reply(int code);

// void get_error_reply_code_description(int type, int code, char *description);

// void sockaddr_to_ipv4_addr_str(struct sockaddr *sockaddr, char *addr);

// void get_src_ipv4_addr_str(void *ip_packet, char *ipv4_addr_str);

// void int_addr_to_ipv4_addr_str(uint32_t addr, char *str_addr);

// void open_socket();

#endif