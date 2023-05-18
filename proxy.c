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

typedef struct Client Client;
#define BUFFER_SIZE 1024

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

/*
 * This function is called when a new connection is noticed on the server
 * socket.
 * The proxy accepts a new connection and creates the relevant data structures.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
int handle_new_client(int server_sock, struct ClientList *list)
{

    printf("handle_new_client\n");
    Client *client = malloc(sizeof(Client));
    if (client == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for a new client.\n");
        return -1;
    }

    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int client_sock = tcp_accept(server_sock, (struct sockaddr *)&client_address, &client_len);

    if (client_sock == -1)
    {
        fprintf(stderr, "Failed to accept a new client.\n");
        free(client);
        return -1;
    }

    // Initialize the client
    client->socket_fd = client_sock;
    client->source = 0;  // source is not known yet
    client->dest_id = 0; // destination is not known yet

    char format_buffer[BUFFER_SIZE];
    int format_received = 0;
    while (format_received <= 0)
    {
        format_received = tcp_read(client_sock, format_buffer, sizeof(format_buffer));
        if (format_received < 0)
        {
            fprintf(stderr, "Failed to read format from a new client.\n");
            close(client_sock);
            free(client);
            return -1;
        }
    }
    for (int i = 0; i < format_received; i++)
    {
        printf("%02x ", format_buffer[i]);
    }
    printf("\n");

    client->format_type = check_format_type(format_buffer[0]);
    printf("format type: %d\n", client->format_type);
    printf("tcp_read returned: %d\n", format_received);
    sleep(10);

    // Add the client to the list
    insert(list, client);

    return client_sock;
}

/*
 * This function is called when a connection is broken by one of the connecting
 * clients. Data structures are clean up and resources that are no longer needed
 * are released.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void remove_client(Client *client, struct ClientList *list)
{
    remove_node(list, client->source);
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
void forward_message(Record *msg, struct ClientList *list)
{
    // Find the client with the matching source
    Client *client = find_client_by_id(list, msg->source);

    // If the client is not found, discard the Record
    if (client == NULL)
    {
        fprintf(stderr, "No client found with that source id: %c\n", msg->source);
        deleteRecord(msg);
        return;
    }

    // Convert the Record to the appropriate format
    char *buffer;
    int bufSize = 0;

    if (client->format_type == 1) // XML format
    {
        buffer = recordToXML(msg, &bufSize);
    }
    else // Binary format
    {
        buffer = recordToBinary(msg, &bufSize);
    }

    if (bufSize <= 0)
    {
        fprintf(stderr, "Error converting the Record to the required format.\n");
        deleteRecord(msg);
        return;
    }

    // Send the converted message to the client
    int bytesSent = tcp_write(client->socket_fd, buffer, bufSize);
    if (bytesSent != bufSize)
    {
        fprintf(stderr, "Failed to send the complete message to the client.\n");
    }

    deleteRecord(msg);
    free(buffer);
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
void handle_client(Client *client, struct ClientList *list)
{
    printf("DO I GET HERE?\n");
    char buffer[1024];
    int bytesRead = tcp_read(client->socket_fd, buffer, sizeof(buffer));
    printf("bytesRead: %d\n", bytesRead);

    if (bytesRead > 0)
    {
        Record *msg = NULL;
        int bytesReadForFormat = sizeof(sizeof(buffer));

        if (client->format_type == 1) // XML format
        {
            msg = XMLtoRecord(buffer, bytesRead, &bytesReadForFormat);
        }
        else
        {
            msg = BinaryToRecord(buffer, bytesRead, &bytesReadForFormat);
        }

        if (msg != NULL)
        {
            forward_message(msg, list);
        }
        else
        {
            fprintf(stderr, "Error converting the received data to a Record.\n");
        }
    }
    else if (bytesRead == 0)
    {
        remove_node(list, client->source);
        remove_client(client, list);
    }
    else
    {
        fprintf(stderr, "Error reading from a client socket.\n");
    }
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
    struct ClientList *clientList = create_client_list();

    fd_set master_fds, read_fds;
    int fd_max;
    int new_client_sock;

    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);
    FD_SET(server_sock, &master_fds);
    fd_max = server_sock;
    printf("Waiting for connections...\n");

    do
    {
        read_fds = master_fds;
        tcp_wait(&read_fds, fd_max);

        for (int i = 0; i <= fd_max; i++)
        {
            if (FD_ISSET(i, &master_fds) && FD_ISSET(i, &read_fds))
            {
                printf("New activity on socket %d\n", i);
                if (i == server_sock)
                {
                    // New connection on the server socket
                    printf("New connection on the server socket\n");
                    new_client_sock = handle_new_client(server_sock, clientList);
                    printf("New client socket: %d\n", new_client_sock);
                    FD_SET(new_client_sock, &master_fds);
                    if (new_client_sock > fd_max)
                    {
                        printf("Updating fd_max\n");
                        fd_max = new_client_sock;
                    }
                }
                else
                {
                    Client *client = find_client_by_id(clientList, i);
                    if (client != NULL)
                    {
                        handle_client(client, clientList);
                    }
                }
            }
        }
    } while (clientList->size > 0);

    /* add your cleanup code */
    delete_client_list(clientList);
    tcp_close(server_sock);

    return 0;
}
