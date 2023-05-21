#ifndef linkedlist_h
#define linkedlist_h

#include "proxy.h"

struct ClientList
{
    struct Client *head;
    int size;
};

struct ClientList *create_client_list();

int check_format_type(char *buffer, int len);

void insert(struct ClientList *list, struct Client *client);

void remove_node(struct ClientList *list, int source);

struct Client *find_client_by_source(struct ClientList *list, int source);

struct Client *find_client_by_socket(struct ClientList *list, int socket_id);

int get_client_format_type(struct ClientList *list, int source);

void delete_client_list(struct ClientList *list);

#endif
