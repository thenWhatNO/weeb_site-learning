#ifndef SERVER_H
#define SERVER_H

#include "client.h"
#include "html_data.h"
#include "datagram.h"
#include "files_m.h"

typedef struct {
    int fd; //soket number
    char *ip;
    struct addrinfo *server_info;
    int status; // 1 running, 0 off
} Server_socket;

int initilize_server(char *ip, char *port, Server_socket *server_sock); 
int clear_server_data(Server_socket *Server_socket);
int start_server(Server_socket *server_sock);
int stop_server(Server_socket *server_sock);
int close_server(Server_socket *server_sock);

int accept_server(Server_socket *server_fd, Clients_socket *client_sock, size_t timeout);
int listin_server(Datagram_store *DTgrams, Server_socket *server_fd ,Clients_socket *client_socket , size_t timeout);
int send_datagram(HTML_datagram *html_data ,Clients_socket *client_socket, char *others);
#endif