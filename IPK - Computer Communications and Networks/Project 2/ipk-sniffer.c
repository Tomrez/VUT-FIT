/******************* proj2.c ***********************
 * Second project for IPK - ZETA - Network sniffer  *
 * Author: Tomas Reznicek                           *
 * Login: xrezni33                                  *
 * Date: 21-04-2024                                 *
 ***************************************************/

#include "ipk-sniffer.h"

void SIGDetected(int sig)
{
    if(sig != 0)
    {
        pcap_close(handle);
        exit(EXIT_SUCCESS_CODE);
    }
}

void argsParse(Arguments *arguments,int argc, char *argv[])
{
    //Possible options for getopt_long
    static struct option options[] = {
    {"interface", optional_argument, 0, 'i'},
    {"port-source", required_argument, 0, 's'},
    {"port-destination", required_argument, 0, 'd'},
    {"tcp", no_argument, 0, 't'},
    {"udp", no_argument, 0, 'u'},
    {"icmp4", no_argument, 0, '4'},
    {"icmp6", no_argument, 0, '6'},
    {"arp", no_argument, 0, 'a'},
    {"ndp", no_argument, 0, 'x'},
    {"igmp", no_argument, 0, 'g'},
    {"mld", no_argument, 0, 'm'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
    };

    if (argc > 1)
    {
        int opt;
        //Go through each argument and parse
        while((opt = getopt_long(argc, argv, "i::tup:n:h", options, NULL)) != -1)
        {
            switch (opt)
            {
                case 'i':
                    if (arguments->interfaceFlag == false)
                    {
                        //Handling spacebar issue
                        if (optarg == NULL && argv[optind] != NULL && argv[optind][0] != '-')
                        {
                            arguments->interface = strdup(argv[optind]);
                            optind++;
                        }
                        else
                        {
                            arguments->interface = optarg;
                        }

                        arguments->interfaceFlag = true;
                    }
                    else
                    {
                        fprintf(stderr, "Cannot give interface argument multiple times\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;

                case 's':
                    if (arguments->portSrc == NULL)
                    {
                        if (optarg)
                        {
                            arguments->portSrc = strdup(optarg);
                            //Value validity check
                            if (atoi(arguments->portSrc) < 1 || atoi(arguments->portSrc) > 65535)
                            {
                                fprintf(stderr, "Wrong port number. Port should be number in <0, 65535>\n");
                                exit(WRONG_ARGUMENT_VALUE_CODE);
                            }
                        }
                        else
                        {
                            fprintf(stderr, "Wrong port argument\n");
                            exit(WRONG_ARGUMENT_VALUE_CODE);
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Cannot give port informations multiple times\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;

                case 'd':
                    if (arguments->portDst == NULL)
                    {
                        if (optarg)
                        {
                            arguments->portDst = strdup(optarg);
                            //Value validity check
                            if (atoi(arguments->portDst) < 1 || atoi(arguments->portDst) > 65535)
                            {
                                fprintf(stderr, "Wrong port number. Port should be number in <0, 65535>\n");
                                exit(WRONG_ARGUMENT_VALUE_CODE);
                            }
                        }
                        else
                        {
                            fprintf(stderr, "Wrong port argument\n");
                            exit(WRONG_ARGUMENT_VALUE_CODE);
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Cannot give port informations multiple times\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;
                
                case 't':
                    if (arguments->tcpFlag == false)
                    {
                        arguments->tcpFlag = true;
                    }
                    else
                    {
                        fprintf(stderr, "Cannot give TCP argument multiple times\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;
                
                case 'u':
                    if (arguments->udpFlag == false)
                    {
                        arguments->udpFlag = true;
                    }
                    else
                    {
                        fprintf(stderr, "Cannot give UDP argument multiple times\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;

                case 'p':
                    if (arguments->port == NULL)
                    {
                        if (optarg)
                        {
                            arguments->port = strdup(optarg);
                            //Value validity check
                            if (atoi(arguments->port) < 1 || atoi(arguments->port) > 65535)
                            {
                                fprintf(stderr, "Wrong port number. Port should be number in <0, 65535>\n");
                                exit(WRONG_ARGUMENT_VALUE_CODE);
                            }
                        }
                        else
                        {
                            fprintf(stderr, "Wrong port argument\n");
                            exit(WRONG_ARGUMENT_VALUE_CODE);
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Cannot give port informations multiple times\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;

                case '4':
                    if (arguments->icmp4Flag == false)
                    {
                        arguments->icmp4Flag = true;
                    }
                    else
                    {
                        fprintf(stderr, "Cannot give icmp4 flag multiple times\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;
                
                case '6':
                    if (arguments->icmp6Flag == false)
                    {
                        arguments->icmp6Flag = true;
                    }
                    else
                    {
                        fprintf(stderr, "Double icmp6 argument not allowed\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;
                
                case 'a':
                    if (arguments->arpFlag == false)
                    {
                        arguments->arpFlag = true;
                    }
                    else
                    {
                        fprintf(stderr, "Double arp argument not allowed\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;
                
                case 'x':
                    if (arguments->ndpFlag == false)
                    {
                        arguments->ndpFlag = true;
                    }
                    else
                    {
                        fprintf(stderr, "Double ndp argument not allowed\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;
                
                case 'g':
                    if (arguments->igmpFlag == false)
                    {
                        arguments->igmpFlag = true;
                    }
                    else
                    {
                        fprintf(stderr, "Double igmp argument not allowed\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;

                case 'm':
                    if (arguments->mldFlag == false)
                    {
                        arguments->mldFlag = true;
                    }
                    else
                    {
                        fprintf(stderr, "Double mld argument not allowed\n");
                        exit(DOUBLE_ARGUMENT_CODE);
                    }
                    break;

                case 'n':
                    arguments->numOfPackets = atoi(optarg);
                    break;

                case 'h':
                    printHelp();
                    break;

                default:
                    fprintf(stderr, "Wrong arguments\n");
                    exit(UNKNOWN_ARGUMENT_CODE); 
            }
        }
    }
}

void printHelp()
{
    printf ("How to execute program correctly: \n\n");
    printf("./ipk-sniffer [-i interface | --interface interface] {-p|--port-source|--port-destination port [--tcp|-t] [--udp|-u]} [--arp] [--ndp] [--icmp4] [--icmp6] [--igmp] [--mld] {-n num}\n");
    printf("\n-i eth0 (just one interface to sniff) or --interface.\n");
    printf("-t or --tcp (will display TCP segments and is optionally complemented by -p or --port-* functionality).\n");
    printf("-u or --udp (will display UDP datagrams and is optionally complemented by-p or --port-* functionality).\n");
    printf("-p (extends previous two parameters to filter TCP/UDP based on port number.\n");
    printf("--port-destination 23 (extends previous two parameters to filter TCP/UDP based on port number.\n");
    printf("--port-source 23 (extends previous two parameters to filter TCP/UDP based on port number.\n");
    printf("--icmp4 (will display only ICMPv4 packets).\n");
    printf("--icmp6 (will display only ICMPv6 echo request/response).\n");
    printf("--arp (will display only ARP frames).\n");
    printf("--ndp (will display only NDP packets, subset of ICMPv6).\n");
    printf("--igmp (will display only IGMP packets).\n");
    printf("--mld (will display only MLD packets, subset of ICMPv6).\n");
    printf("-n 10 (specifies the number of packets to display)\n");
    printf("Unless protocols are explicitly specified, all (i.e., all content, regardless of protocol) are considered for printing.\n");
    printf("All arguments can be in any order.\n");
    
    exit(EXIT_SUCCESS_CODE);
}

void activeInterfaces (Arguments *arguments)
{
    //If -i was given -> active interfaces
    if (arguments->interface == NULL && arguments->interfaceFlag == true)
    {
        struct ifaddrs *ifap, *ifa;

        if (getifaddrs(&ifap) == -1)
        {
            fprintf(stderr, "getifaddrs error\n");
            exit(INTERNAL_ERROR_CODE);
        }

        printf("List of active interfaces:\n");
        for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr == NULL)
            {
                continue;
            }
            if (ifa->ifa_addr->sa_family == AF_INET)
            {
                struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
                printf("- Interface Name: %s, Address: %s\n", ifa->ifa_name, inet_ntoa(addr->sin_addr));
            }
        }

        freeifaddrs(ifap);
        exit(EXIT_SUCCESS_CODE);
    }
    //If not -> print help
    printHelp();
}

void filterString(char *string, Arguments *arguments)
{
    //Flag, if opening bracket was needed 
    bool openBracket = false;

    //TCP and UDP argument
    if (arguments->tcpFlag && arguments->udpFlag)
    {
        strcat(string, "(tcp or udp ");
        if (arguments->icmp4Flag || arguments->icmp6Flag || arguments->arpFlag || arguments->igmpFlag)
        {
            strcat(string, "or ");
        }
        openBracket = true;
    } 
    else if (arguments->tcpFlag)
    {
        strcat(string, "(tcp ");
        if (arguments->icmp4Flag || arguments->icmp6Flag || arguments->arpFlag || arguments->igmpFlag)
        {
            strcat(string, "or ");
        }
        openBracket = true;
    }
    else if (arguments->udpFlag)
    {
        strcat(string, "(udp ");
        if (arguments->icmp4Flag || arguments->icmp6Flag || arguments->arpFlag || arguments->igmpFlag)
        {
            strcat(string, "or ");
        }
        openBracket = true;
    }
    //ICMP4 and ICMP6 arguments and all possible combinations
    if (arguments->icmp4Flag && arguments->icmp6Flag)
    {
        if (arguments->ndpFlag && arguments->mldFlag)
        {
            strcat(string, "icmp or (icmp6 and (icmp6[0] == 135 or icmp6[0] == 143)) ");
        }
        else if (arguments->ndpFlag)
        {
            strcat(string, "icmp or (icmp6 and icmp6[0] == 135) ");
        }
        else if (arguments->mldFlag)
        {
            strcat(string, "icmp or (icmp6 and icmp6[0] == 143) ");
        }
        else
        {
            strcat(string, "icmp or icmp6 ");
        }
        
        if (arguments->arpFlag || arguments->igmpFlag)
        {
            strcat(string, "or ");
        }
    }
    else if (arguments->icmp4Flag)
    {
        strcat(string, "icmp ");

        if (arguments->ndpFlag && arguments->mldFlag)
        {
           strcat(string, "or icmp6[0] == 135 or icmp6[0] == 143 ");
        }
        else if (arguments->ndpFlag)
        {
            strcat(string, "or icmp6[0] == 135 ");
        }
        else if (arguments->mldFlag)
        {
            strcat(string, "or icmp6[0] == 143 ");
        }

        if (arguments->arpFlag || arguments->igmpFlag)
        {
            strcat(string, "or ");
        }
    }
    else if (arguments->icmp6Flag)
    {
       //subsets for ICMP6 -> have to be AND between each of them
        if (arguments->ndpFlag)
        {
            strcat(string, "icmp6 and icmp6[0] == 135 ");
        }
        else if (arguments->mldFlag)
        {
            strcat(string, "icmp6 and icmp6[0] == 143 ");
        }
        else
        {
            strcat(string, "icmp6 ");
        }
        if (arguments->arpFlag || arguments->igmpFlag)
        {
            strcat(string, "or ");
        }
    }
    else if (arguments->ndpFlag)
    {
        if (arguments->mldFlag)
        {
            strcat(string, "icmp6[0] == 135 and icmp6[0] == 143 ");
        }
        else
        {
            strcat(string, "icmp6[0] == 135 ");
        }
        if (arguments->arpFlag || arguments->igmpFlag)
        {
            strcat(string, "or ");
        }
    }
    else if (arguments->mldFlag)
    {
        strcat(string, "icmp6[0] == 143 ");
        if (arguments->arpFlag || arguments->igmpFlag)
        {
            strcat(string, "or ");
        }
    }
    //ARP argument
    if (arguments->arpFlag)
    {
        strcat(string, "arp ");
        if (arguments->igmpFlag)
        {
            strcat(string, "or ");
        }
    }
    //IGMP argument
    if (arguments->igmpFlag)
    {
        strcat(string, "igmp ");
    }

    //If opening bracket was written, closing one must be written as well
    if (openBracket)
    {
        strcat(string, ") ");
    }
    //If any packet filter was given, there must be and between that and port filter
    if ((arguments->port || arguments->portDst || arguments->portSrc) && (arguments->tcpFlag || arguments->udpFlag || arguments->icmp4Flag || arguments->icmp6Flag || arguments->mldFlag || arguments->ndpFlag || arguments->arpFlag || arguments->igmpFlag))
    {
        strcat(string, "and ");
    }
    //Port filter
    if (arguments->port != NULL)
    {
        strcat(string, "port ");
        strcat(string, arguments->port);
        
        if (arguments->portDst != NULL || arguments->portSrc != NULL)
        {
            strcat(string, " or ");
        }
        else
        {
            strcat(string, " ");
        }
    }
    //Source port filter
    if (arguments->portSrc != NULL)
    {
        strcat(string, "src port ");
        strcat(string, arguments->portSrc);

        if (arguments->portDst != NULL)
        {
            strcat(string, " or ");
        }
        else
        {
            strcat(string, " ");
        }
    }
    //Destination port filter
    if (arguments->portDst != NULL)
    {
        strcat(string, "dst port ");
        strcat(string, arguments->portDst);
    }
}


char* getTimestamp(struct timeval timestamp)
{
    // Convert timestamp to time_t
    time_t sec = timestamp.tv_sec;
    int usec = timestamp.tv_usec;

    // time_t to tm
    struct tm *timeinfo = localtime(&sec);

    // Get right format of timestamp
    char *timestampString = (char *)malloc(30 * sizeof(char));
    if (timestampString == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(INTERNAL_ERROR_CODE);
    }

    // Date and time
    strftime(timestampString, 30, "%Y-%m-%dT%H:%M:%S", timeinfo);
    //Microseconds
    sprintf(timestampString + 19, ".%03d", usec);
    //Timezone
    strftime(timestampString + 26, 6, "%z", timeinfo);

    return timestampString;
}


void packetHandler(u_char *param, const struct pcap_pkthdr *header, const u_char *packetData)
{
    //1 - TCP, 2 - UDP, 0 - else
    int tcpUdpFlag = 0;

    // UDP x TCP x ELSE check
    const struct iphdr *ipHeader = (struct iphdr*)(packetData + ETH_HEADER_LENGTH);
    uint8_t protocol = ipHeader->protocol;
    
    if (protocol == IPPROTO_TCP)
    {
        tcpUdpFlag = 1;
    }
    else if (protocol == IPPROTO_UDP)
    {
        tcpUdpFlag = 2;
    }

    // Get right timestamp format, print it and free allocated memory
    char *timestamp = getTimestamp(header->ts);
    printf("\ntimestamp: %s\n", timestamp);
    free(timestamp);

    // Print source and destination MAC adress 
    printf("src MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", packetData[6], packetData[7], packetData[8], packetData[9], packetData[10], packetData[11]);
    printf("dst MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", packetData[0], packetData[1], packetData[2], packetData[3], packetData[4], packetData[5]);

    // Print length of frame
    printf("frame length: %d bytes\n", header->len);

    // Print source and destination IP address
    printf("src IP: %d.%d.%d.%d\n", packetData[26], packetData[27], packetData[28], packetData[29]);
    printf("dst IP: %d.%d.%d.%d\n", packetData[30], packetData[31], packetData[32], packetData[33]);

    if (tcpUdpFlag == 1)
    {
        // Print TCP source and destination port numbers
        uint16_t src_port = (packetData[34] << 8) | packetData[35];
        uint16_t dest_port = (packetData[36] << 8) | packetData[37];

        printf("src port: %u\n", src_port);
        printf("dst port: %u\n\n", dest_port); 
    }
    else if (tcpUdpFlag == 2)
    {
        //Print UDP source and destination port numbers
        const u_char *udpHeader = packetData + ETH_HEADER_LENGTH + IP_HEADER_LENGTH;
    
        uint16_t srcPort = (udpHeader[0] << 8) | udpHeader[1];
        uint16_t dstPort = (udpHeader[2] << 8) | udpHeader[3];

        printf("src port: %u\n", srcPort);
        printf("dst port: %u\n\n", dstPort);
    }
    else
    {
        printf("\n");
    }

    for (unsigned int i = 0; i < header->len; i += 16)
    {
        // Offset
        printf("0x%04x: ", i);

        // Hexadecimal
        for (unsigned int j = i; j < i + 16 && j < header->len; j++)
        {
            printf("%02x ", packetData[j]);
        }

        // Padding - every missing byte get 3 spaces
        for (unsigned int j = header->len - i; j < 16; j++)
        {
            printf("   ");
        }

        printf(" ");

        // ASCII
        for (unsigned int j = i; j < i + 16 && j < header->len; j++)
        {
            if (packetData[j] >= 32 && packetData[j] <= 126)
            {
                printf("%c", packetData[j]);
            } 
            else
            {
                printf(".");
            }
        }
        printf("\n");
    }
    printf("\n");

    // I dont use this arg, but to solve Unused warning i do this
    if (param != NULL)
    {
        param = NULL;
    }
}


int main(int argc, char *argv[])
{
    // Struct to store arguments
    Arguments arguments = {NULL, false, NULL, NULL, NULL, false, false, false, false, false, false, false, false, 1};

    // Parse args and store them into arguments var
    argsParse(&arguments, argc, argv);

    // If interface wasnt given, print active interfaces
    if (arguments.interface == NULL)
    {
        activeInterfaces(&arguments);
    }

    char errorBuffer[PCAP_ERRBUF_SIZE];

    // Handle is global variable, reasons are described in header file
    // Device, max packet size, promiscuit mode, timeout (-1 will wait forever), errorMsg buffer
    handle = pcap_open_live(arguments.interface, BUFSIZ, 1, 1000, errorBuffer);
    if (handle == NULL)
    {
        fprintf(stderr, "Error opening device: %s\n", errorBuffer);
        exit(WRONG_INTERFACE_CODE);
    }

    //C-c signal handler
    signal(SIGINT, SIGDetected);

    struct bpf_program fp;

    // Allocate memory for filter string and set every position to \0
    char *filter = malloc(1000*sizeof(char));
    if (filter == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(INTERNAL_ERROR_CODE);
    }
    memset(filter, '\0', 1000*sizeof(char));

    // Generate filter string from parsed arguments
    filterString(filter, &arguments);

    // Try to compice with filter
    if (pcap_compile(handle, &fp, filter, 0, PCAP_NETMASK_UNKNOWN) == -1)
    {
        fprintf(stderr, "Error compiling filter: %s\n", pcap_geterr(handle));
        free(filter);
        free(arguments.interface);
        free(arguments.port);
        free(arguments.portDst);
        free(arguments.portSrc);

        exit(FORBIDEN_COMBINATION_OR_COMPILE_ERROR_CODE);
    }

    free(filter);
    free(arguments.interface);
    free(arguments.port);
    free(arguments.portDst);
    free(arguments.portSrc);

    // Try to set compiled filter
    if (pcap_setfilter(handle, &fp) == -1)
    {
        fprintf(stderr, "Error setting filter: %s\n", pcap_geterr(handle));
        exit(FORBIDEN_COMBINATION_OR_COMPILE_ERROR_CODE);
    }

    // Ready to read packets
    // NumOfPackets determines, how many packets will be parsed
    pcap_loop(handle, arguments.numOfPackets, packetHandler, NULL);

    // If numOfPackets was given, it will get here after getting n packets, if not -> never
    pcap_close(handle);

    return EXIT_SUCCESS_CODE;
}