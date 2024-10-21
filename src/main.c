#include <stdio.h>

#include "tr_data.h"
#include "resources.h"
#include "net.h"
#include "traceroute.h"

static void print_man() {
    printf("Usage: %s --help | HOST\n", g_tr_data.executable_name);
    printf("%s - print the route packets trace to network host.\n", g_tr_data.executable_name);
    printf("Options:\n");
    printf("--help give this help list\n");
}

int main(int ac, char **av)
{
    set_defaults();
    parse_args(ac, av);

    if (g_tr_data.print_man_only)
    {
        print_man();
        exit(EXIT_SUCCESS);
    }

    init_resouces();
    
    get_addr_info(g_tr_data.target_host_arg, &g_resources.target_addr_info);

    sockaddr_to_ipv4_addr_str(g_resources.target_addr_info->ai_addr, g_tr_data.target_addr);

    open_send_socket();
    open_receive_socket();
    set_recv_timeout(g_resources.receive_socket, &g_tr_data.probe_timeout);

    traceroute();

    free_resources();
    exit(EXIT_SUCCESS);
}

// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <netinet/ip.h>  // For IP header
// #include <netinet/udp.h> // For UDP header
// #include <unistd.h>      // For close()

// // Pseudo-header for UDP checksum calculation
// struct pseudo_header {
//     uint32_t src_addr;
//     uint32_t dst_addr;
//     uint8_t placeholder;
//     uint8_t protocol;
//     uint16_t udp_length;
// };

// // Function to calculate checksum
// unsigned short checksum(void *b, int len) {
//     unsigned short *buf = b;
//     unsigned int sum = 0;
//     unsigned short result;

//     for (sum = 0; len > 1; len -= 2)
//         sum += *buf++;
//     if (len == 1)
//         sum += *(unsigned char *)buf;
//     sum = (sum >> 16) + (sum & 0xFFFF);
//     sum += (sum >> 16);
//     result = ~sum;
//     return result;
// }

// // Function to calculate UDP checksum
// unsigned short udp_checksum(struct iphdr *iph, struct udphdr *udph, char *data, int data_len) {
//     char buffer[4096];
//     struct pseudo_header psh;

//     // Fill in the pseudo header
//     psh.src_addr = iph->saddr;
//     psh.dst_addr = iph->daddr;
//     psh.placeholder = 0;
//     psh.protocol = IPPROTO_UDP;
//     psh.udp_length = htons(sizeof(struct udphdr) + data_len);

//     // Copy the pseudo header to the buffer
//     memcpy(buffer, &psh, sizeof(struct pseudo_header));

//     // Copy the UDP header to the buffer
//     memcpy(buffer + sizeof(struct pseudo_header), udph, sizeof(struct udphdr));

//     // Copy the UDP data to the buffer
//     memcpy(buffer + sizeof(struct pseudo_header) + sizeof(struct udphdr), data, data_len);

//     // Compute the checksum
//     return checksum(buffer, sizeof(struct pseudo_header) + sizeof(struct udphdr) + data_len);
// }

// static void compare(void *banchmark, void *my, int length)
// {
//     int i = 0;
//     while (i < length)
//     {
//         uint8_t bm = *( ((uint8_t*)banchmark) + i);
//         uint8_t my_val = *( ((uint8_t*)my) + i);
//         printf("bm %X my %X\n", bm, my_val);
//         i++;
//     }
// }

// // Function to send UDP packet via raw socket
// void send_udp_raw(const char *src_ip, const char *dst_ip, uint16_t src_port, uint16_t dst_port, const char *message) {
//     int sockfd;
//     char buffer[4096];
//     struct sockaddr_in dest_addr;
//     struct iphdr *iph = (struct iphdr *) buffer;
//     struct udphdr *udph = (struct udphdr *) (buffer + sizeof(struct iphdr));

//     int payload_len = 4;
//     int packet_len = sizeof(struct iphdr) + sizeof(struct udphdr) + payload_len;

//     // Create a raw socket
//     sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
//     if (sockfd < 0) {
//         perror("socket creation failed");
//         exit(EXIT_FAILURE);
//     }

//     // Zero out the buffer
//     memset(buffer, 0, sizeof(buffer));
    
//     uint8_t ttl = 3;
    
//     // Fill in the IP header
//     iph->ihl = 5;  // IP header length (5 * 4 = 20 bytes)
//     iph->version = 4;  // IPv4
//     iph->tos = 0;  // Type of service
//     iph->tot_len = htons(packet_len);  // Total packet length
//     iph->id = htons(123);  // Packet ID (random)
//     iph->frag_off = 0;  // Fragment offset
//     iph->ttl = ttl;  // Time to live
//     iph->protocol = IPPROTO_UDP;  // UDP protocol
//     iph->saddr = inet_addr(src_ip);  // Source IP address
//     iph->daddr = inet_addr(dst_ip);  // Destination IP address
//     iph->check = 0;  // Checksum set to 0 before calculating
//     iph->check = checksum((unsigned short *)buffer, sizeof(struct iphdr));  // Calculate IP checksum

//     // Fill in the UDP header
//     udph->source = htons(src_port);  // Source port
//     udph->dest = htons(dst_port);  // Destination port
//     udph->len = htons(sizeof(struct udphdr) + payload_len);  // UDP length
//     udph->check = 0;  // Checksum set to 0 initially

//     // Copy the message to the packet (payload)
//     memcpy(buffer + sizeof(struct iphdr) + sizeof(struct udphdr), message, payload_len);

//     // Calculate UDP checksum
//     udph->check = udp_checksum(iph, udph, (char *)buffer + sizeof(struct iphdr) + sizeof(struct udphdr), payload_len);

//     // Set up destination address structure
//     memset(&dest_addr, 0, sizeof(dest_addr));
//     dest_addr.sin_family = AF_INET;
//     dest_addr.sin_addr.s_addr = iph->daddr;
    
//     struct iphdr ip_header = create_ip_header(ttl, inet_addr(src_ip), inet_addr(dst_ip), sizeof(struct s_udp_frame));
    
//     struct s_udp_frame upd_frame = create_udp_frame(&ip_header, 12345, 33434, 0XAABBCCDD);
//     struct s_ip_frame ip_frame;
//     ip_frame.header = ip_header;
//     ip_frame.body = upd_frame;
    
//     compare(buffer + 20, (char*)&upd_frame, (int) sizeof(struct s_udp_frame));



//     // Send the packet
//     if (sendto(sockfd, &ip_frame, sizeof(struct s_ip_frame), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
//         perror("sendto failed");
//         close(sockfd);
//         exit(EXIT_FAILURE);
//     }

//     printf("UDP packet sent successfully.\n");

//     // Close the socket
//     close(sockfd);
// }

// int main() {
//     const char *src_ip = "192.168.43.57";  // Replace with your source IP
//     const char *dst_ip = "142.251.36.238";  // Replace with your destination IP
//     uint16_t src_port = 12345;
//     uint16_t dst_port = 33434;
//     uint32_t msg = 0XAABBCCDD;

//     send_udp_raw(src_ip, dst_ip, src_port, dst_port, (char*)&msg);

//     return 0;
// }
