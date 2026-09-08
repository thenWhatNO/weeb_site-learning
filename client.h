#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>

#define MAX_CLIENTS 1000

typedef struct {
    char command[100];
    int active;
    int is_cl;
} Clients_socket;

// create the client socket strutc
int init_client_sock(Clients_socket *client_sock);

// add new client to the struct
int add_client(Clients_socket *client_sock, int fd);
int add_command_to_client(Clients_socket *client_sock, int fd, char *command);

int last_open_socket(Clients_socket *client_sock);

int remove_client(Clients_socket *client_sock,int fd);

int clear_client_socket(Clients_socket *client_sock);

#endif