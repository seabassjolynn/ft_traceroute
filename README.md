# ft_traceroute
School 42 project. Reimplementation of traceroute.
## Some random details ##
1. traceroute can send 3 types of probes: UDP(by default), TCP, ICMP. When UDP is sent, then it is sent to a likely unusable port with  
hight number like 33443, this is because UDP packet shouldn't be handled by the host so host will reply with ICMP error reply `Port unreachabe` and  
hence traceroute will know on receiving of such reply, that host was reached. With TCP, the port number doesn't matter, because host will reply first with  
either TCP: SYN-ACK (if port is opened) or SYN-RST before any TCP communication starts. In case of ICMP, echo reply will come back to traceroute, so again, no  
probe packet handling will take place.
2. When traceroute uses udp (by default) it sends udp frames with different ports. For the 1st TTL, that make 3 probes, traceroute uses one dst port (strat port is 33435), for the  
next 3 it uses 1st dst port + 1 etc (this is at least how it works on debian 12).
3. On debian 12 traceroute sends probe, whats for an answer, then sends probe etc.
## Use Cases

<details>
  <summary>Use Case 1: tracing Route to a Remote Server</summary>

  **Description**: This use case demonstrates how to trace the route to a remote server, such as `10.0.81.131`.
  
  **Details**:
  - **TTL** - 1, 2, 3 - are TTLs of the UDP packet sent to host
  - **round trip time** - traceroute sends by default 3 probes for a TTL. the time between sending of probe and receiving reply to a probe  
  is a round trip time. Below traceroute sends 3 probes with TTL = 1, and displays 3 round trip times for each probe: 13.929ms  4.154ms  2.038ms
  
  **Command and output**:
    
    ```bash
    traceroute 10.0.81.131
    1   10.0.2.2  0.245ms  0.225ms  0.079ms 
    2   192.168.43.1  13.929ms  4.154ms  2.038ms 
    3   10.0.81.131  31.877ms  20.503ms  40.334ms

</details>
<details>
  <summary>Use case 2: star is displayed if the probe doesn't get response</summary>
    
  **Description:** If the answer (ICMP error reply - time to live exceeded) is not received back, then start is displayed instead of round trip time

        ```bash```
        traceroute google.com
        traceroute to google.com (142.251.36.238), 64 hops max
        1   10.0.2.2  0.176ms  0.106ms  0.130ms 
        2   192.168.43.1  6.921ms  2.566ms  2.447ms 
        3   10.0.81.131  53.025ms  21.549ms  27.415ms 
        4   10.81.102.92  30.508ms  38.622ms  24.494ms 
        5   *  *  * 
        6   62.53.206.66  52.642ms  19.375ms  34.817ms 
        7   62.53.13.83  68.262ms  28.104ms  19.700ms 
        8   72.14.219.226  45.127ms  19.250ms  19.797ms 
        9   192.178.105.123  39.196ms  24.409ms  29.427ms 
        10  108.170.228.33  31.935ms  22.503ms  17.897ms 
        11  142.251.36.238  25.186ms  29.515ms  34.781ms 

  **Response to one probe is not received**
    
        ```bash```
        traceroute to google.com (142.251.36.238), 64 hops max
        1   10.0.2.2  0.326ms  0.280ms  0.159ms 
        2   192.168.43.1  5.388ms  2.109ms  1.669ms 
        3   10.0.81.130  32.848ms  35.072ms  17.855ms 
        4   10.81.102.90  29.596ms  36.426ms  34.302ms 
        5   *  10.81.85.22  41.662ms  26.309ms 
        6   62.53.206.66  38.134ms  20.294ms  30.536ms 
        7   62.53.13.83  38.385ms  28.914ms  20.122ms 
        8   72.14.219.226  40.342ms  14.813ms  23.791ms 
        9   192.178.105.123  32.925ms  26.350ms  38.407ms 
        10   108.170.228.33  48.501ms  34.751ms  29.510ms 
        11   142.251.36.238  28.919ms  31.447ms  29.909ms
  **Another variation**
    
    ```bash```
    8   62.53.6.127  26.723ms  *  23.750ms
</details>
<details>
    <summary>If the probe answers come from different gateways, the address of each responding system will be printed.</summary>

    ```bash```
    1  192.168.1.1 (192.168.1.1)  1.123 ms  1.234 ms  1.345 ms
    2  10.0.0.1 (10.0.0.1)  5.678 ms  5.789 ms  6.012 ms
    3  192.0.2.1 (192.0.2.1)  10.123 ms  10.456 ms  10.789 ms
    4  203.0.113.1 (203.0.113.1)  15.234 ms  15.456 ms
    203.0.113.2 (203.0.113.2)  16.123 ms
    5  198.51.100.1 (198.51.100.1)  20.345 ms  20.567 ms  20.789 ms

</details>
<details>
    <summary>After some predefined amout of probe sets with different ttls and if host is still not reached, no failure details are printed</summary>

    ```bash```
    52   *  *  * 
    53   *  *  * 
    54   *  *  * 
    55   *  *  * 
    56   *  *  * 
    57   *  *  * 
    58   *  *  * 
    59   *  *  * 
    60   *  *  * 
    61   *  *  * 
    62   *  *  * 
    63   *  *  * 
    64   *  *  *
</details>
<details>
    <summary>If hop is encounered that doesn't send back `time to live exceeded` after 3 probes, then utility continues to try new probes with increased ttl and  
    doesn't fails</summary>
    *this is because there are hosts that pass on the packet, but doesn't reply with ICMP time to live exceeded*

    ```bash```
    traceroute www.akamai.com
    traceroute to e259222.dscx.akamaiedge.net (23.38.123.177), 64 hops max
    1   10.0.2.2  0.183ms  0.027ms  0.084ms 
    2   192.168.43.1  32.558ms  3.950ms  1.979ms 
    3   10.0.81.130  29.278ms  24.964ms  38.483ms 
    4   10.81.102.88  45.813ms  28.630ms  33.669ms 
    5   *  *  * 
    6   62.53.206.66  45.596ms  36.277ms  30.889ms 
    7   62.53.13.83  31.498ms  36.900ms  30.792ms 
    8   194.59.190.59  18.651ms  37.981ms  20.738ms 
    9   *  *  * 
    10  *  *  * 
    11   23.38.123.177  21.155ms  33.411ms  31.249ms
</details>
<details>
    <summary>-i option, the probe packets can be directed through specific network interface of hops</summary>
</details>
<details>
    <summary>-m max number of hops</summary>
</details>
<details>
    <summary>-q nqueries number of queries per hop</summary>
</details>
<details>
    <summary></summary>
</details>
<details>
    <summary></summary>
</details>
<details>
    <summary></summary>
</details>
