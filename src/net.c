#include "net.h"

void get_addr_info(char *host, struct addrinfo **addr_info)
{
    struct addrinfo hints;
    ft_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // Allow IPv4
    hints.ai_socktype = SOCK_DGRAM; // Raw socket for ICMP
    hints.ai_protocol = IPPROTO_UDP; // ICMP protocol
    
    int result = getaddrinfo(host, NULL, &hints, addr_info);

    if (result != 0) {
        fprintf(stderr, "%s: unknown host\n", g_tr_data.executable_name);
        free_resources();
        exit(EXIT_FAILURE);
    }
}

struct timeval current_time()
{
    struct timeval time;
    int get_time_result = gettimeofday(&time, NULL);
    if (get_time_result == -1)
    {
        fprintf(stderr, "Error when getting time stamp for ping. Error: %s\n", strerror(errno));
        free_resources();
        exit(EXIT_FAILURE);
    }
    return time;
}

struct pseudo_header {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint8_t placeholder;
    uint8_t protocol;
    uint16_t udp_length;
};

static uint16_t calculate_checksum(void *addr, int len)
{    
    uint32_t sum = 0;
    while (len > 1)
    {
        sum += *((uint16_t *) addr);  
        addr += 2;
        len -= 2;
    }

    if( len > 0 )
    {
        sum += * (uint8_t*) addr;
    }
               
    /*  Fold 32-bit sum to 16 bits */
    while (sum >> 16)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
}

static unsigned short udp_checksum(struct iphdr *iph, void *udp_frame, uint16_t udp_frame_len) 
{
    char buffer[4096];
    struct pseudo_header psh;

    // Fill in the pseudo header
    psh.src_addr = iph->saddr;
    psh.dst_addr = iph->daddr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(udp_frame_len);

    // Copy the pseudo header to the buffer
    ft_memcpy(buffer, &psh, sizeof(struct pseudo_header));

    // Copy the UDP header to the buffer
    ft_memcpy(buffer + sizeof(struct pseudo_header),udp_frame, udp_frame_len);

    // Compute the checksum
    return calculate_checksum(buffer, sizeof(struct pseudo_header) + udp_frame_len);
}

struct s_udp_frame create_udp_frame(struct iphdr *iphdr, uint16_t src_port, uint16_t dst_port, uint32_t data)
{
    struct s_udp_frame udp_frame;
    udp_frame.header.src_port = htons(src_port);
    udp_frame.header.dst_port = htons(dst_port);
    udp_frame.header.total_length = htons(sizeof(struct s_udp_frame));
    udp_frame.data = data;
    
    udp_frame.header.checksum = 0;
    
    udp_frame.header.checksum = udp_checksum(iphdr, &udp_frame, (uint16_t)sizeof(struct s_udp_frame));
    return udp_frame;
}

struct iphdr create_ip_header(uint8_t ttl, uint32_t src_ip_net, uint32_t dst_ip_net, uint16_t payload_len)
{
    struct iphdr hdr;
    
    hdr.ihl = 5;
    hdr.version = 4;
    hdr.tos = 0;
    hdr.tot_len = htons(sizeof(struct iphdr) + payload_len);
    hdr.id = htons(123); //id used to collect fragments of ip frame if ip frame is split, here arbitrary num is used
    hdr.frag_off = 0;
    hdr.ttl = ttl;
    hdr.protocol = IPPROTO_UDP;
    hdr.check = 0;
    hdr.saddr = src_ip_net;
    hdr.daddr = dst_ip_net;
    hdr.check = calculate_checksum(&hdr, sizeof(struct iphdr));
    return hdr;
}

void open_send_socket()
{
    g_resources.send_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (g_resources.send_socket == -1) 
    {
        perror("send socket");
        free_resources();
        exit(EXIT_FAILURE);
    }
}

void open_receive_socket()
{
    g_resources.receive_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_resources.receive_socket == -1) {
        perror("receive socket");
        free_resources();
        exit(EXIT_FAILURE);
    }
}

void set_ttl(int socket, int ttl)
{
    if (setsockopt(socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) 
    {
        perror("error when configuring send socket with setsockopt");
        free_resources();
        exit(EXIT_FAILURE);
    }
}

void set_recv_timeout(int socket, struct timeval *timeout)
{
    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const void*)timeout, sizeof(struct timeval)) != 0)
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
        exit(EXIT_FAILURE);
    }
}

static void int_addr_to_ipv4_addr_str(uint32_t addr, char *str_addr) 
{
    struct in_addr in_addr;
    in_addr.s_addr = addr;
    ft_memset(str_addr, 0, INET_ADDRSTRLEN);
    
    if(inet_ntop(AF_INET,&in_addr, str_addr, INET_ADDRSTRLEN) == NULL) {
        fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
        free_resources();
        exit(EXIT_FAILURE);
    }
}

void get_src_addr(void *ip_packet, char *ipv4_addr_str)
{
    struct iphdr ip_header = *((struct iphdr *) ip_packet); 
    uint32_t addr = ip_header.saddr;
    int_addr_to_ipv4_addr_str(addr, ipv4_addr_str);
}

static void convert_ip_to_string(uint32_t ip, char *str) {
    struct in_addr ip_addr;
    ip_addr.s_addr = htonl(ip);  // Convert to network byte order
    inet_ntop(AF_INET, &ip_addr, str, INET_ADDRSTRLEN);
}

uint32_t get_local_ip_net_byte_order() 
{
    int sock;
    struct sockaddr_in remote_addr;
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    struct timeval timeout;
    int ttl;

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        close(sock);
        exit(EXIT_FAILURE);
    }
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const void*)&timeout, sizeof(struct timeval)) != 0)
    {
        perror("setsockopt");
        close(sock);
        free_resources();
        exit(EXIT_FAILURE);
    }
    
    ttl = 3; //low TTL is chosen to speed up the function (the 3rd node will send us a response). In response the node will return ICMP error reply or echo reply. We are fine with either of them as we will need only dst ip of ip header.
    
    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) 
    {
        perror("setsockopt");
        close(sock);
        free_resources();
        exit(EXIT_FAILURE);
    }

    ft_memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "8.8.8.8", &remote_addr.sin_addr);  // Google DNS
    
    struct s_icmp_echo_frame icmp_frame = create_echo_request();
    
    int sendResult = sendto(sock, &icmp_frame, sizeof(struct s_icmp_echo_frame), 0, (struct sockaddr*)&remote_addr, addr_len);
    
    if (sendResult < 0) 
    {
        fprintf(stderr, "Error when sending ICPM packet. Error: %s\n", strerror(errno));
        close(sock);
        free_resources();
        exit(EXIT_FAILURE);
    }
    
    uint8_t received_ip_packet[4096];
    ft_memset(received_ip_packet, 0, 4096);
    
    sendResult = recvfrom(sock, received_ip_packet, 4096, 0, NULL, NULL);
    if (sendResult < 0)
    {
        fprintf(stderr, "Error when receiving CPM packet. Error: %s\n", strerror(errno));
        close(sock);
        free_resources();
        exit(EXIT_FAILURE);
    }
    close(sock);
    struct iphdr header = *((struct iphdr *) received_ip_packet); 

    char ip[INET_ADDRSTRLEN];
    
    convert_ip_to_string(header.daddr, ip);

    return header.daddr;
}

struct s_icmp_echo_frame create_echo_request()
{
    struct s_icmp_echo_frame frame;
    
    frame.header.type = ECHO_REQUEST_TYPE;
    frame.header.code = 0;
    frame.header.checksum = 0;
    frame.header.sequence_number = 1;
    frame.header.identifier = 1;
    ft_memset(&frame.payload, 'A', sizeof(frame.payload));
    frame.header.checksum = calculate_checksum(&frame, sizeof(struct s_icmp_echo_frame));
    return frame;
}
