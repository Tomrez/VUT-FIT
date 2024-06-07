/******************* proj2.h ***********************
 * Second project for IPK - ZETA - Network sniffer  *
 * Author: Tomas Reznicek                           *
 * Login: xrezni33                                  *
 * Date: 21-04-2024                                 *
 ***************************************************/

#include "stdio.h"
#include <pcap.h>
#include "stdlib.h"
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>

//EXIT CODES DEFINITION
#define EXIT_SUCCESS_CODE 0
#define INTERNAL_ERROR_CODE 99
#define UNKNOWN_ARGUMENT_CODE 12
#define DOUBLE_ARGUMENT_CODE 13
#define WRONG_ARGUMENT_VALUE_CODE 14
#define FORBIDEN_COMBINATION_OR_COMPILE_ERROR_CODE 15
#define WRONG_INTERFACE_CODE 16

//Typical length of ETH header
#define ETH_HEADER_LENGTH 14

//just for IPv4
#define IP_HEADER_LENGTH 20

/**
 * Struct to store parsed arguments to
 * Inicialize values when creating instance
*/
typedef struct
{
    char* interface;
    bool interfaceFlag;
    char *port;
    char *portSrc;
    char *portDst;
    bool tcpFlag;
    bool udpFlag;
    bool arpFlag;
    bool ndpFlag;
    bool icmp4Flag;
    bool icmp6Flag;
    bool igmpFlag;
    bool mldFlag;
    int numOfPackets;
}Arguments;

/**
 * Global variable handle
 * I decide to use global variable because of correct pcap_close when SIGINT happens
*/
pcap_t *handle;

/**
 * Funcion will parse each input argument and store them into Arguments structure given via first argument
 * @param Arguments* Instance of structure Arguments, in which parsed arguments will be stored
 * @param int Number of arguments
 * @param char** Array of input arguments
*/
void argsParse ( Arguments*, int, char** );

/**
 * Function will print help and exit with success code
*/
void printHelp();

/**
 * Function will write all active interfaces and exit program with success exit code
 * @param Arguments* Instance of structure Atguments, in which parsed arguments are stored
*/
void activeInterfaces( Arguments* );

/**
 * Function will make the sring used for setting filters from given arguments
 * @param char* Alocated memory, in which the result string will be stored
 * @param Arguments* Instance of structure Arguments, in which parsed arguments are stored
*/
void filterString( char*, Arguments* );

/**
 * Function will get timestamp in requied format from timestamp in header of packet
 * @param struct timeval Input timestamp
 * @return char* Return value is string with the formated timestamp
*/
char* getTimestamp( struct timeval );

/**
 * Function will get each packet which go through filter and print informations on standard output STDOUT
 * @param u_char* Aditional parameters given from function, where pcap_loop was called. In this case always zero
 * @param const struct pcap_pkthdr* Header of packet
 * @param const u_char* Data from packet body
*/
void packetHandler( u_char*, const struct pcap_pkthdr*, const u_char* );