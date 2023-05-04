#include "proxy.h"
#include <stdio.h>
#include <stdlib.h>

struct ClientList
{
    struct Client *head;
};

int check_format_type(char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Could not open file%s\n", filename);
        exit(EXIT_FAILURE);
    }

    int symbol = fgetc(file);
    fclose(file);

    if (symbol == '<')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void insert(struct ClientList *list, int source, int dest_id, int format_type, char *filename)
{
    struct Client *client = (struct Client *)malloc(sizeof(struct Client));
    client->source = source;
    client->dest_id = dest_id;
    client->format_type = check_format_type(filename);
    client->next = list->head;
    list->head = client;
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
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }
}

struct Client *find_client_by_dest_id(struct ClientList *list, int dest_id)
{
    struct Client *tmp = list->head;
    while (tmp != NULL)
    {
        if (tmp->dest_id == dest_id)
        {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

int get_client_format_type(struct ClientList *list, int source)
{
    struct Client *client = find_client_by_dest_id(list, source);
    if (client == NULL)
    {
        return -1;
    }
    return client->format_type;
}