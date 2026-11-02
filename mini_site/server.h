#ifndef SERVER_H
#define SERVER_H

typedef struct Server_socket Server_socket;

Server_socket *initilize_server(char *ip, char *port); 
int start_server(Server_socket *server_sock);
int stop_server(Server_socket *server_sock);
int close_server(Server_socket *server_sock);
int accept_server(Server_socket *server_fd, int timeout);

int free_server(Server_socket *server_fd);
#endif