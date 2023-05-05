#include "proxy.h"
#include <stdio.h>
#include <stdlib.h>

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
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    list->size--;
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
