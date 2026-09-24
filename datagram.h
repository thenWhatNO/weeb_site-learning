#ifndef DATAGRAM_H
#define DATAGRAM_H

typedef struct Datagram Datagram;
typedef struct Datagram_store Datagram_store;
typedef struct Packeg Packeg;

Datagram_store *init_datagramstore();
int add_datagram(Datagram_store *datagram, char *data, int clientfd, int data_size); 
int free_datagram_store(Datagram_store *datagram);
Packeg *read_datagram(Datagram *dg);
int free_datagram(Packeg *dg);

// int generate_html_chat () : generate the html pyje;
int read_client_msg(Datagram_store *DTgrams, HTML_datagram *html_data, Files_struct *myfiles, Clients_socket *client_sock);

#endif