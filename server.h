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

#define MAIN_HADR "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: %zu\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n%s"
#define ALIVE_HADR "HTTP/1.1 200 OK\r\nContent-Type: text/event-stream\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n"
#define MAEG_HADR "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s"

typedef struct {
    int fd; //soket number
    char *ip;
    struct addrinfo *server_info;
    int status; // 1 running, 0 off
} Server_socket;

typedef struct {
    FILE *html_file;
    FILE *chat_file;
} Files_struct;

// this is for the listen_server function to hendel 
// moltupal datagrams at once

typedef struct {
    char *msg;
    int client_id;
    int msg_size;
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
    int updated;
} HTML_datagram; 

int init_datagramstore(Datagram_store *datagram);
int add_datagram(Datagram_store *datagram, char *data, int clientfd, int data_size);
int read_datagrams(Datagram_store *datagram); 
int clear_datagram(Datagram_store *datagram);

// int generate_html_chat () : generate the html pyje;
int read_client_msg(Datagram_store *DTgrams, HTML_datagram *html_data, Files_struct *myfiles);


int initilize_server(char *ip, char *port, Server_socket *server_sock); 
int start_server(Server_socket *server_sock);
int stop_server(Server_socket *server_sock);
int close_server(Server_socket *server_sock);
int clear_server_data(Server_socket *Server_socket);

int accept_server(Server_socket *server_fd, Clients_socket *client_sock, size_t timeout);
int listin_server(Datagram_store *DTgrams, Server_socket *server_fd ,Clients_socket *client_socket , size_t timeout);
int send_datagram(Server_socket *server_fd, HTML_datagram *html_data ,Clients_socket *client_socket);

// ---------------- html data struffs
int generate_data(HTML_datagram *html_data, Files_struct *myfiles, Datagram_store *DTgrams, char *hadr);
int generate_msg(Files_struct *myfiles, char **buff);

int init_html_struct(HTML_datagram *html_str);
int write_msg_to_html(HTML_datagram *html_data, char *msg, size_t msg_len);
int free_html_stract(HTML_datagram *html_data);


//     working with files
int open_files(Files_struct *myfiles);
int close_files(Files_struct *myfiles);
int update_chat_file(Files_struct *myfiles, char *data, size_t d_size);
int read_file(Files_struct *myfiles, char file, char **buffer);

#endif