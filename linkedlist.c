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

void insert(struct ClientList *list, struct Client *client)
{
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

struct Client *find_client_by_id(struct ClientList *list, int source)
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
