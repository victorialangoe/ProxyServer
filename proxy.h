#ifndef PROXY_H
#define PROXY_H

struct Client
{
    int source;
    int dest_id;
    int format_type;
    int socket_fd;
    struct Client *next;
};

#endif
