#ifndef linkedlist_h
#define linkedlist_h

#include "proxy.h"

struct ClientList
{
    struct Client *head;
};

int check_format_type(char *filename);

void insert(struct ClientList *list, struct Client *client);

void remove_node(struct ClientList *list, int source);

struct Client *find_client_by_id(struct ClientList *list, int dest_id);

int get_client_format_type(struct ClientList *list, int source);

#endif
