#ifndef SERVER_H
#define SERVER_H

typedef struct Server_socket Server_socket;

int initilize_server(char *ip, char *port, Server_socket *server_sock); 
int clear_server_data(Server_socket *Server_socket);
int start_server(Server_socket *server_sock);
int stop_server(Server_socket *server_sock);
int close_server(Server_socket *server_sock);

int accept_server(Server_socket *server_fd, Clients_socket *client_sock, size_t timeout);
int listin_server(Datagram_store *DTgrams, Server_socket *server_fd ,Clients_socket *client_socket , size_t timeout);
int global_sand(int *client_socket, char *data, size_t data_l);
#endif