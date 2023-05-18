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
#include <sys/time.h>
// TODO: remove unused imports

int check_error(int res, char *msg)
{
    if (res == -1)
    {
        perror(msg);
        return -1;
    }
    return 0;
}

int tcp_connect(char *hostname, int port)
{
    int socket_fd, rc;
    struct addrinfo hints, *res, *resp;
    char servname[6];

    sprintf(servname, "%d", port);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    rc = getaddrinfo(hostname, servname, &hints, &res);
    if (rc != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        exit(EXIT_FAILURE);
    }

    for (resp = res; resp != NULL; resp = resp->ai_next)
    {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1)
        {
            perror("socket");
            continue;
        }

        rc = connect(socket_fd, resp->ai_addr, resp->ai_addrlen);
        if (rc == 0)
        {
            break; // successfully connected
        }
        else
        {
            perror("connect");
            close(socket_fd); // close the socket and try the next
        }
    }

    if (resp == NULL)
    {
        fprintf(stderr, "failed to connect\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);

    return socket_fd;
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
    int socket_fd, rc;
    struct sockaddr_in address;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    check_error(socket_fd, "socket");

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    rc = bind(socket_fd, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
    check_error(rc, "bind");

    rc = listen(socket_fd, SOMAXCONN);
    check_error(rc, "listen");

    return socket_fd;
}

int tcp_accept(int server_sock, struct sockaddr *restrict address, socklen_t *restrict address_len)
{
    int socket_fd = accept(server_sock, address, address_len);
    check_error(socket_fd, "accept");

    return socket_fd;
}

int tcp_wait(fd_set *waiting_set, int wait_end)
{
    int rc = select(wait_end + 1, waiting_set, NULL, NULL, NULL);
    check_error(rc, "select");
    return rc;
}

int tcp_wait_timeout(fd_set *waiting_set, int wait_end, int timeout)
{
    struct timeval wait_time;
    wait_time.tv_sec = timeout;
    wait_time.tv_usec = 0;

    int rc = select(wait_end + 1, waiting_set, NULL, NULL, &wait_time);
    check_error(rc, "select");
    return rc;
}

