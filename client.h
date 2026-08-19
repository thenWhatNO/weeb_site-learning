#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>

typedef struct {
    int *fds;
    int *ides;
    int cap;
    int size; 

} Client_socket;

// create the client socket strutc
int init_client_sock(Client_socket *client_sock);

// add new client to the struct
int add_client(Client_socket *client_sock,int fd, int id);

int remove_client(Client_socket *client_sock,int fd, int id);

int clear_client_socket(Client_socket *client_sock);

int update_clients_fd(Client_socket *client_sock);

#endif