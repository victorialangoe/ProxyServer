/*
 * This is the main program for the proxy, which receives connections for sending and receiving clients
 * both in binary and XML format. Many clients can be connected at the same time. The proxy implements
 * an event loop.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "xmlfile.h"
#include "connection.h"
#include "record.h"
#include "recordToFormat.h"
#include "recordFromFormat.h"
#include "linkedlist.h"
#include "proxy.h"

#include <arpa/inet.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

/* This struct should contain the information that you want
 * keep for one connected client.
 */

typedef struct Client Client;

void usage(char *cmd)
{
    fprintf(stderr, "Usage: %s <port>\n"
                    "       This is the proxy server. It takes as imput the port where it accepts connections\n"
                    "       from \"xmlSender\", \"binSender\" and \"anyReceiver\" applications.\n"
                    "       <port> - a 16-bit integer in host byte order identifying the proxy server's port\n"
                    "\n",
            cmd);
    exit(-1);
}

int extract_dest_id_from_file_content(char *content, int format_type)
{
    int dest_id = -1;
    if (format_type == 1) // XML format
    {
        char *start = strstr(content, "<dest=");
        if (start != NULL)
        {
            start += 6;
            dest_id = *start;
        }
    }
    else if (format_type == 0) // Binary format
    {
        if ((content[0] & 0x02) != 0) // Check if the 'has_dest' flag is set
        {
            dest_id = content[2];
        }
    }

    return dest_id;
}

char extract_source_from_file_content(char *content, int format_type)
{
    char source = '\0';
    if (format_type == 1) // XML format
    {
        char *start = strstr(content, "<source=");
        if (start != NULL)
        {
            start += 9; // Move 9 characters ahead to reach the value of the source attribute
            source = *start;
        }
    }
    else if (format_type == 0) // Binary format
    {
        if ((content[0] & 0x01) != 0) // Check if the 'has_source' flag is set
        {
            source = content[1];
        }
    }

    return source;
}

/*
 * This function is called when a new connection is noticed on the server
 * socket.
 * The proxy accepts a new connection and creates the relevant data structures.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
int handle_new_client(int server_sock, char *filename, struct ClientList *list)
{
    Client *client = malloc(sizeof(Client));
    if (client == NULL)
    {
        fprintf(stderr, "Failed to allocate memory to a new client.\n");
        return -1;
    }
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int client_sock = tcp_accept(server_sock, (struct sockaddr *)&client_address, &client_len);
    int format_type = check_format_type(filename);

    char buffer[1024];

    int dest_id = extract_dest_id_from_file_content(buffer, format_type);
    char source = extract_source_from_file_content(buffer, format_type);

    if (source != '\0' && dest_id != -1)
    {
        client->source = source;
        client->dest_id = dest_id;
        client->format_type = format_type;
        insert(list, client);
    }
    else
    {
        // Handle the case when there's no 'source' or 'dest' in the file content
        // You can add your code here
        free(client); // Don't forget to free the memory if you don't insert the client into the list.
    }

    return client_sock;
}

/*
 * This function is called when a connection is broken by one of the connecting
 * clients. Data structures are clean up and resources that are no longer needed
 * are released.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void remove_client(Client *client)
{
    tcp_close(client->socket_fd);
    memset(client, 0, sizeof(Client));
}

/*
 * This function is called when the proxy received enough data from a sending
 * client to create a Record. The 'dest' field of the Record determines the
 * client to which the proxy should send this Record.
 *
 * If no such client is connected to the proxy, the Record is discarded without
 * error. Resources are released as appropriate.
 *
 * If such a client is connected, this functions find the correct socket for
 * sending to that client, and determines if the Record must be converted to
 * XML format or to binary format for sendig to that client.
 *
 * It does then send the converted messages.
 * Finally, this function deletes the Record before returning.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void forward_message(Record *msg)
{
}

/*
 * This function is called whenever activity is noticed on a connected socket,
 * and that socket is associated with a client. This can be sending client
 * or a receiving client.
 *
 * The calling function finds the Client structure for the socket where acticity
 * has occurred and calls this function.
 *
 * If this function receives data that completes a record, it creates an internal
 * Record data structure on the heap and calls forwardMessage() with this Record.
 *
 * If this function notices that a client has disconnected, it calls removeClient()
 * to release the resources associated with it.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void handle_client(Client *client)
{
}

int main(int argc, char *argv[])
{
    int port;
    int server_sock;

    if (argc != 2)
    {
        usage(argv[0]);
    }

    port = atoi(argv[1]);

    server_sock = tcp_create_and_listen(port);
    if (server_sock < 0)
        exit(-1);

    /* add your initialization code */

    /*
     * The following part is the event loop of the proxy. It waits for new connections,
     * new data arriving on existing connection, and events that indicate that a client
     * has disconnected.
     *
     * This function uses handleNewClient() when activity is seen on the server socket
     * and handleClient() when activity is seen on the socket of an existing connection.
     *
     * The loops ends when no clients are connected any more.
     */
    do
    {
        /* fill in your code */
    } while (1 /* fill in your termination condition */);

    /* add your cleanup code */

    tcp_close(server_sock);

    return 0;
}
