#ifndef CLIENT_H
#define CLIENT_H

#define MAX_CLIENTS 20

#include <stddef.h>

typedef struct Clients_socket Clients_socket;

// create the client socket strutc
Clients_socket *init_client_sock();

// add new client to the struct
int add_client(Clients_socket *client_sock, int fd);
int add_command_to_client(Clients_socket *client_sock, int fd, char *command);

int last_open_socket(Clients_socket *client_sock);

int remove_client(Clients_socket *client_sock, int fd);
int view_client_alive(Clients_socket *client_sock, int fd);
int global_sand(Clients_socket *client_socket, char *data, size_t data_l);

int is_client_true(Clients_socket *client_sock, int index);

int view_clients_status(Clients_socket *client_sock);

#endif