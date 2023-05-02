struct Client
{
    int source;
    int dest_id;
    int format_type;
    int socket_fd;
    struct Client* next;
};