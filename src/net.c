#include "net.h"

void get_addr_info(char *host, struct addrinfo **addr_info)
{
    struct addrinfo hints;
    ft_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // Allow IPv4
    hints.ai_socktype = SOCK_RAW; // Raw socket for ICMP
    hints.ai_protocol = IPPROTO_UDP; // ICMP protocol
    
    int result = getaddrinfo(host, NULL, &hints, addr_info);

    if (result != 0) {
        fprintf(stderr, "%s: unknown host\n", g_tr_data.executable_name);
        free_resources();
        exit(ERROR);
    }
}


void open_send_socket()
{
    g_resources.send_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (g_resources.send_socket == -1) {
        perror("send socket");
        free_resources();
        exit(ERROR);
    }
}

void open_receive_socket()
{
    g_resources.receive_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_resources.receive_socket == -1) {
        perror("receive socket");
        free_resources();
        exit(ERROR);
    }
}

void confugure_send_scocket()
{
    if (setsockopt(g_resources.send_socket, IPPROTO_IP, IP_TTL, &g_tr_data.max_hops, sizeof(g_tr_data.max_hops)) != 0) 
    {
        perror("error when configuring send socket with setsockopt");
        free_resources();
        exit(ERROR);
    }
}

void confugure_receive_scocket()
{
    if (setsockopt(g_resources.receive_socket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&g_tr_data.probe_timeout, sizeof(struct timeval)) != 0)
    {
        perror("error when configuring receive socket with setsockopt");
        free_resources();
        exit(EXIT_FAILURE);
    }
}

void sockaddr_to_ipv4_addr_str(struct sockaddr *sockaddr, char *addr)
{
    //here i cast struct sockaddr* to struct sockaddr_in* because ai_addr may contain different address structures: struct sockaddr_in* - for ipv4, struct sockaddr_in6* for ipv6
    struct sockaddr_in sockaddr_in = *(struct sockaddr_in*) sockaddr;

    if(inet_ntop(AF_INET,&sockaddr_in.sin_addr, addr, INET_ADDRSTRLEN) == NULL) {
        
        fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
        free_resources();
        exit(ERROR);
    }
}

// int get_ip_header_length_in_bytes(uint8_t *packet)
// {
//     return (packet[0] & 0XF) * 4; //ip header length value is a length measured in 32 bit words, so we multiply for 4 to find the length in bytes. 
// }

// uint16_t caclulate_checksum(void *addr, int len)
// {    
//     uint32_t sum = 0;
//     while (len > 1)
//     {
//         sum += *((uint16_t *) addr);  
//         addr += 2;
//         len -= 2;
//     }

//     if( len > 0 )
//     {
//         sum += * (uint8_t*) addr;
//     }
               

//     /*  Fold 32-bit sum to 16 bits */
//     while (sum >> 16)
//     {
//         sum = (sum & 0xffff) + (sum >> 16);
//     }
//     return ~sum;
// }

// // struct s_icmp_echo_packet create_echo_request()
// // {
// //     struct s_ping_header header;
// //     struct s_icmp_echo_packet packet;
    
// //     header.type = ECHO_REQUEST_TYPE;
// //     header.code = 0;
// //     header.checksum = 0;
// //     header.sequence_number = -1;
// //     header.identifier = g_ping_session.id;
    
// //     packet.header = header;
// //     memset(&packet.payload, 'A', sizeof(packet.payload));
// //     return packet;
// // }

// bool is_ping_error_reply(int code)
// {
//     int types[5] = {3, 4, 5, 11, 12};
//     int i = 0;
//     while (i < 5)
//     {
//         if (types[i] == code)
//         {
//             return true;
//         }
//         i++;
//     }
//     return false;
// }

// void get_error_reply_code_description(int type, int code, char *description)
// {
//     switch (type) {
//         case 3: // Destination Unreachable (RFC 792)
//             switch (code) {
//                 case 0:
//                     strcpy(description, "Destination Net Unreachable");
//                     break;
//                 case 1:
//                     strcpy(description, "Destination Host Unreachable");
//                     break;
//                 case 2:
//                     strcpy(description, "Protocol Unreachable");
//                     break;
//                 case 3:
//                     strcpy(description, "Port Unreachable");
//                     break;
//                 case 4:
//                     strcpy(description, "Fragmentation Needed and Don't Fragment was Set");
//                     break;
//                 case 5:
//                     strcpy(description, "Source Route Failed");
//                     break;
//                  default:
//                     strcpy(description, "Unknown Code");
//                     break;
//             }
//             break;
//         case 4: // Source Quench (Deprecated, RFC 6633)
//             strcpy(description, "Source Quench");
//             break;
//         case 5: // Redirect (RFC 792)
//             switch (code) {
//                 case 0:
//                     strcpy(description, "Redirect Datagram for the Network");
//                     break;
//                 case 1:
//                     strcpy(description, "Redirect Datagram for the Host");
//                     break;
//                 case 2:
//                     strcpy(description, "Redirect Datagram for the Type of Service and Network");
//                     break;
//                 case 3:
//                     strcpy(description, "Redirect Datagram for the Type of Service and Host");
//                     break;
//                 default:
//                     strcpy(description, "Unknown Code");
//                     break;
//             }
//             break;
//         case 11: // Time Exceeded (RFC 792)
//             switch (code) {
//                 case 0:
//                     strcpy(description, "Time to live exceeded");
//                     break;
//                 case 1:
//                     strcpy(description, "Fragment reassembly time exceeded");
//                     break;
//                 default:
//                     strcpy(description, "Unknown Code");
//                     break;
//             }
//             break;
//         case 12: // Parameter Problem (RFC 792)
//             switch (code) {
//                 case 0:
//                     strcpy(description, "Pointer indicates the error");
//                     break;
//                 default:
//                     strcpy(description, "Unknown Code");
//                     break;
//             }
//             break;
//         default:
//             strcpy(description, "Unknown Type");
//             break;
//     }
// }

// void get_src_ipv4_addr_str(void *ip_packet, char *ipv4_addr_str)
// {
//     struct s_ip_header ip_header = *((struct s_ip_header *) ip_packet); 
//     uint32_t addr = ip_header.src_address;
//     int_addr_to_ipv4_addr_str(addr, ipv4_addr_str);
// }

// void int_addr_to_ipv4_addr_str(uint32_t addr, char *str_addr) 
// {
//     struct in_addr in_addr;
//     in_addr.s_addr = addr;
//     bzero(str_addr, INET_ADDRSTRLEN);
    
//     if(inet_ntop(AF_INET,&in_addr, str_addr, INET_ADDRSTRLEN) == NULL) {
//         fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
//         free_resources();
//         exit(ERROR);
//     }
// }

// void configure_socket()
// {
//     if (setsockopt(g_resources.fd_socket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&g_ping_session.flags.linger, sizeof(struct timeval)) != 0)
//     {
//         perror("setsockopt");
//         free_resources();
//         exit(EXIT_FAILURE);
//     }
    
//     if (setsockopt(g_resources.fd_socket, IPPROTO_IP, IP_TTL, &g_ping_session.flags.ttl, sizeof(g_ping_session.flags.ttl)) != 0) {
//         perror("setsockopt");
//         free_resources();
//         exit(EXIT_FAILURE);
//     }
// }