#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "client.h"


typedef struct {
    int fd; //soket number
    char *ip;
    struct addrinfo *server_info;
    int status; // 1 running, 0 off
} Server_socket;

// this is for the listen_server function to hendel 
// moltupal datagrams at once

typedef struct {
    char *msg;
    int client_id;
    //todo add time :3
} Datagram;

typedef struct {
    int size;
    int cap;
    Datagram *datagrams;
} Datagram_store;

typedef struct{
    char *html_msg;
    int len;
    int cap;
} HTML_datagram; 

int init_datagramstore(Datagram_store *datagram);
int add_datagram(Datagram_store *datagram, char *data, int clientfd);
int clear_datagram(Datagram_store *datagram);

// int generate_html_chat () : generate the html pyje;
int generate_data(HTML_datagram *html_data, size_t data_len, Datagram_store *DTgrams);
int read_client_msg(Datagram_store *DTgrams);

int start_server(Server_socket *server_sock);
int initilize_server(char *ip, char *port, Server_socket *server_sock); 
int stop_server(Server_socket *server_sock);
int close_server(Server_socket *server_sock);
int clear_server_data(Server_socket *Server_socket);

int accept_server(Server_socket *server_fd, Client_socket *client_sock, size_t timeout);

// return -1 if get en error, return 0 is ervything is fine
int listin_server(Datagram_store *DTgrams, Server_socket *server_fd ,int *client_set ,size_t client_len , size_t timeout);

// send individualy for one client a datagram
int send_datagram(Server_socket *server_fd, HTML_datagram *html_data ,int *client_set, size_t client_len, size_t dg_size);

// up date client when reconnecting or diconnecting
int update_clients(int *clients_set);

// ---------------- html data struffs


int init_html_struct(HTML_datagram *html_str);
int write_msg_to_html(HTML_datagram *html_data, char *msg, size_t msg_len);
int free_html_stract(HTML_datagram *html_data);

#endif