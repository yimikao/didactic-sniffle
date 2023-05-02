#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "wnet.h"


// pending conns queue will hold
#define BACKLOG 5

// first get addrinfo
// create a socket from its properties
// bind that socket to the network interface of these properties
int get_listener_or_die(char *port)
{
    int sockfd;
    struct addrinfo match, 
    *servinfo,
    // potential interface
    *first_match;
    // use to affirm/set value of socket options below
    int yes;

    // check local network interfaces and try to match address requirements
    memset(&match, 0, sizeof match);
    // unspecified, either IPv4/v6
    match.ai_family = AF_UNSPEC;
    // TCP 
    match.ai_socktype = SOCK_STREAM;
    // Any IP, my IP
    match.ai_flags = AI_PASSIVE;
    
    // convert IP into socket addresses
    // get linked list of addresses/interfaces (servinfo) with match
    int code = getaddrinfo(NULL, port, &match, &servinfo);
    if (code != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(code));
        return -1;
    }

    // loop through each and try to create a socket with this info. quit looping at first success
    for(first_match = servinfo; first_match != NULL; first_match = first_match->ai_next) {
        // try to make socket based on candidate interface
       if ((sockfd = socket(first_match->ai_family, first_match->ai_socktype, first_match->ai_protocol))== -1 )
       {
            //continue on to the next
            continue;
       }

        // set a socket option
        // allow an open socket (TIME_WAIT state) be reused(bound no another NI. easy for testing)
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            // on error, die
            perror("setsockopt");
            close(sockfd);
            freeaddrinfo(servinfo);
            return -2;
        }

        // try to bind the socket to this local IP address
        if (bind(sockfd, first_match->ai_addr, first_match->ai_addrlen) == -1)
        {
            close(sockfd);
            continue;
        }
        // we got a socket (any first one)
        break;
    }

    freeaddrinfo(servinfo);
    if (first_match == NULL)
    {
        fprintf(stderr, "webserver: failed to find local address to use\n");
        return -3;
    }

    // allow remote computers connect to this socket through its IP
    if (listen(sockfd, BACKLOG) == -1) 
    {
        close(sockfd);
        return -4;
    }
    return sockfd;
}