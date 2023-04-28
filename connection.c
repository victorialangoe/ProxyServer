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

int tcp_connect( char* hostname, int port )
{
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_read( int sock, char* buffer, int n )
{
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_write( int sock, char* buffer, int bytes )
{
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_write_loop( int sock, char* buffer, int bytes )
{
    /* TO BE IMPLEMENTED */
    return 0;
}

void tcp_close( int sock )
{
    /* TO BE IMPLEMENTED */
}

int tcp_create_and_listen( int port )
{
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_accept( int server_sock )
{
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_wait( fd_set* waiting_set, int wait_end )
{
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_wait_timeout( fd_set* waiting_set, int wait_end, int seconds )
{
    /* TO BE IMPLEMENTED */
    return 0;
}

