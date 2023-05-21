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
struct ClientList *clientList = NULL;

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

    // Initialize the client
    client->socket_fd = client_sock;
    client->source = 0;       // we check source later
    client->format_type = -1; // we check the format type later

    char format_buffer[BUFFER_SIZE];
    int format_received = 0;
    while (format_received <= 0)
    {
        format_received = tcp_read(client_sock, format_buffer, sizeof(format_buffer));
        if (format_received >= 2)
        {
            // Print the first two bytes as hexadecimal values
            client->format_type = (unsigned char)format_buffer[0];
            client->source = (unsigned char)format_buffer[1];
        }
        if (format_received < 0)
        {
            fprintf(stderr, "Failed to read format from a new client.\n");
            close(client_sock);
            free(client);
            return -1;
        }
    }
    printf("client->format_type: %c\n", client->format_type);
    printf("client->source: %c\n", client->source);

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
void remove_client(Client *client, struct ClientList *list, fd_set *master_fds)
{
    FD_CLR(client->socket_fd, master_fds);
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
    Client *client = find_client_by_source(list, msg->dest);
    if (msg->has_dest)
    {
        fprintf(stderr, "forward_message to: %c\n", msg->dest);
    }
    else
    {
        fprintf(stderr, "forward_message: no destination\n");
    }
    // Convert the Record to the appropriate format
    char *buffer;
    int bufSize = 0;

    if (client->format_type == 'X') // XML format
    {
        buffer = recordToXML(msg, &bufSize);
    }
    else if (client->format_type == 'B') // Binary format
    {
        buffer = recordToBinary(msg, &bufSize);
    }
    else
    {
        fprintf(stderr, "Unknown format type: %c\n", client->format_type);
        deleteRecord(msg);
        return;
    }

    if (bufSize <= 0)
    {
        fprintf(stderr, "Error converting the Record to the required format.\n");
        deleteRecord(msg);
        return;
    }

    // Send the converted message to the client
    int bytesSent = tcp_write_loop(client->socket_fd, buffer, bufSize);
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
void handle_client(Client *client, struct ClientList *list, fd_set *master_fds)
{

    char buffer[BUFFER_SIZE];
    int bytesRead = tcp_read(client->socket_fd, buffer, sizeof(buffer));

    if (bytesRead > 0)
    {

        Record *msg = NULL;
        int bytesReadForFormat = sizeof(sizeof(buffer));

        if (client->format_type == 'X')
        {
            msg = XMLtoRecord(buffer, bytesRead, &bytesReadForFormat);
        }
        else if (client->format_type == 'B')
        {
            msg = BinaryToRecord(buffer, bytesRead, &bytesReadForFormat);
        }
        else
        {
            fprintf(stderr, "Error: Format type was invalid\n");
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
    else
    {
        printf("Removing client\n");
        remove_node(list, client->source);
        remove_client(client, list, master_fds);
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
    {
        exit(-1);
    }

    clientList = create_client_list();

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

        printf("Currently %d client(s) connected.\n", clientList->size);
        for (int i = 0; i <= fd_max; i++)
        {
            if (FD_ISSET(i, &read_fds)) // changed this from master_fds to read_fds
            {
                if (i == server_sock)
                {
                    // New connection on the server socket
                    new_client_sock = handle_new_client(server_sock, clientList);
                    FD_SET(new_client_sock, &master_fds);
                    if (new_client_sock > fd_max)
                    {
                        fd_max = new_client_sock;
                    }
                }
                else
                {
                    printf("Handling client\n");
                    Client *client = find_client_by_socket(clientList, i);
                    if (client != NULL)
                    {
                        handle_client(client, clientList, &master_fds);
                    }
                    else
                    {
                        fprintf(stderr, "No client found with that socket id in main proxy: %d\n", i);
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
