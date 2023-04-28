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

void check_error(int res, char *msg)
{
    if (res == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE); // TODO: find out if I can do it like this or return -1
    }
}

int tcp_connect(char *hostname, int port)
{
    int socketfd, rc;
    struct addrinfo hints, *res, *rp;
    char servname[6];

    sprintf(servname, "%d", port);

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    check_error(socketfd, "socket");

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    rc = getaddrinfo(hostname, servname, &hints, &res);
    if (rc != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        exit(EXIT_FAILURE);
    }

    for (rp = res; rp != NULL; rp = rp->ai_next)
    {
        rc = connect(socketfd, rp->ai_addr, rp->ai_addrlen);
        check_error(rc, "connect");
    }

    freeaddrinfo(res);

    return socketfd;
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
    int wc = write(sock, &buffer, bytes);
    check_error(wc, "write");
    return wc;
}

int tcp_write_loop(int sock, char *buffer, int bytes)
{
    int wc = 0;
    while (wc < bytes + 1)
    {
        wc += write(sock, &buffer, bytes - wc);
        check_error(wc, "write");
    }
    return wc;
}

void tcp_close(int sock)
{
    close(sock);
}

int tcp_create_and_listen(int port)
{
    int socketfd, rc;
    struct sockaddr_in address;

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    check_error(socketfd, "socket");

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    rc = bind(socketfd, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
    check_error(rc, "bind");

    rc = listen(socketfd, SOMAXCONN);
    check_error(rc, "listen");

    return socketfd;
}

int tcp_accept(int server_sock)
{
    int socketfd = accept(server_sock, NULL, NULL);
    check_error(socketfd, "accept");

    return socketfd;
}

int tcp_wait(fd_set *waiting_set, int wait_end)
{
    int rc = select(wait_end + 1, waiting_set, NULL, NULL, NULL);
    check_error(rc, "select");

    return rc;
}

int tcp_wait_timeout(fd_set *waiting_set, int wait_end, int seconds)
{
    struct timeval timeval;
    timeval.tv_sec = seconds;
    int rc = select(wait_end + 1, waiting_set, NULL, NULL, &timeval);
    check_error(rc, "select");

    return rc;
}
