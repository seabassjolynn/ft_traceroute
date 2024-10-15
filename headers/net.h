#ifndef NET_H
#define NET_H

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "exit.h"
#include "utils.h"
#include "resources.h"
#include "tr_data.h"
#include <errno.h> //errno
#include <string.h> //strerror

void get_addr_info(char *host, struct addrinfo **addr_info);

void open_send_socket();

void open_receive_socket();

void confugure_send_scocket();

void confugure_receive_scocket();

void sockaddr_to_ipv4_addr_str(struct sockaddr *sockaddr, char *addr);

void print_traceroute_header();
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

// #define ECHO_REQUEST_TYPE 8

// #define ECHO_REQUEST_CODE 0

// #define ECHO_REPLY_TYPE 0

// #define ECHO_REPLY_CODE 0

// #define ORIGINAL_DATAGRAM_FRAGMENT_LENGTH_IN_ERROR_REPLY 64

// #define TIME_OUT -1

// # include <stdint.h>
// # include <stdbool.h>

// struct s_ping_header
// {
//     uint8_t type;
//     uint8_t code;
//     uint16_t checksum;
//     uint16_t identifier;
//     uint16_t sequence_number;
// };

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