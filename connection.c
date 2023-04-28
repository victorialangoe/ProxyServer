/*
 * This is a file that implements the operation on TCP sockets that are used by
 * all of the programs used in this assignment.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>
// TODO: remove unused imports

#define BUFFSIZE 255

void check_error(int res, char *msg)
{
    if (res == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

int tcp_connect(char *hostname, int port)
{
    int fd, rc;
    struct addrinfo hints, *res, *rp;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    check_error(fd, "socket");

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    rc = getaddrinfo(hostname, port, &hints, &res);
    if (rc != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        exit(EXIT_FAILURE);
    }

    for (rp = res; rp != NULL; rp = rp->ai_next)
    {
        rc = connect(fd, rp->ai_addr, rp->ai_addrlen);
        check_error(rc, "connect");
    }

    freeaddrinfo(res);

    return fd;
}

int tcp_read(int sock, char *buffer, int n)
{
    int rc;
    rc = read(sock, buffer, n);
    check_error(rc, "read");

    return rc;
}

int tcp_write(int sock, char *buffer, int bytes)
{
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_write_loop(int sock, char *buffer, int bytes)
{
    /* TO BE IMPLEMENTED */
    return 0;
}

void tcp_close(int sock)
{
    /* TO BE IMPLEMENTED */
}

int tcp_create_and_listen(int port)
{
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_accept(int server_sock)
{
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_wait(fd_set *waiting_set, int wait_end)
{
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_wait_timeout(fd_set *waiting_set, int wait_end, int seconds)
{
    /* TO BE IMPLEMENTED */
    return 0;
}
