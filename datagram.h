#ifndef DATAGRAM_H
#define DATAGRAM_H

#include "html_data.h"
#include "html_data.h"
#include "files_m.h"
#include "client.h"

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

int init_datagramstore(Datagram_store *datagram);
int add_datagram(Datagram_store *datagram, char *data, int clientfd, int data_size); 
int clear_datagram(Datagram_store *datagram);

// int generate_html_chat () : generate the html pyje;
int read_client_msg(Datagram_store *DTgrams, HTML_datagram *html_data, Files_struct *myfiles, Clients_socket *client_sock);

#endif