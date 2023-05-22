#include "proxy.h"
#include <stdio.h>
#include <stdlib.h>

#define TYPE_NOT_FOUND 2

struct ClientList
{
    struct Client *head;
    int size;
};

struct ClientList *create_client_list()
{
    struct ClientList *list = (struct ClientList *)malloc(sizeof(struct ClientList));
    if (list == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for the client list.\n");
        exit(EXIT_FAILURE);
    }
    list->head = NULL;
    list->size = 0;
    return list;
}

int check_format_type(char *buffer, int len)
{
    int hasNullByte = 0;
    for (int i = 0; i < len; i++)
    {
        if ((unsigned char)buffer[i] == '<')
        {
            return 1; // XML format
        }
        if ((unsigned char)buffer[i] == '\0')
        {
            hasNullByte = 1; // potentially binary format
        }
    }

    if (hasNullByte)
    {
        return 0; // binary format
    }
    else
    {
        return TYPE_NOT_FOUND; // format not found
    }
}

void insert(struct ClientList *list, struct Client *client)
{
    client->next = list->head;
    list->head = client;
    list->size++;
}

void remove_node(struct ClientList *list, int source)
{
    struct Client *tmp = list->head;
    struct Client *prev = NULL;
    while (tmp != NULL)
    {
        if (tmp->source == source)
        {
            if (prev == NULL)
            {
                list->head = tmp->next;
            }
            else
            {
                prev->next = tmp->next;
            }

            free(tmp);
            list->size--;
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }
}

struct Client *find_client_by_source(struct ClientList *list, char source)
{
    struct Client *tmp = list->head;
    while (tmp != NULL)
    {
        if (tmp->source == source)
        {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

struct Client *find_client_by_socket(struct ClientList *list, int socket_id)
{

    struct Client *tmp = list->head;
    while (tmp != NULL)
    {
        if (tmp->socket_fd == socket_id)
        {
            return tmp;
        }
        tmp = tmp->next;
    }

    return NULL;
}

void delete_client_list(struct ClientList *list)
{
    struct Client *current = list->head;
    struct Client *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    free(list);
}
