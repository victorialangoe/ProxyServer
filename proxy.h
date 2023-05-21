#ifndef PROXY_H
#define PROXY_H

struct Client
{
    char source;
    int format_type;
    int socket_fd;
    struct Client *next;
};

#endif
