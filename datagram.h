#ifndef DATAGRAM_H
#define DATAGRAM_H

typedef struct Datagram Datagram;

int get_datagram_client_fd(Datagram *DG);
int get_datagram_msg_size(Datagram *DG);
char *get_datagram_msg(Datagram *DG);

typedef struct Datagram_store Datagram_store;

int get_datagram_store_size(Datagram_store *Dgs);
Datagram *get_datagram_store_datagram(Datagram_store *Dgs, int indx);

typedef struct Packeg Packeg;

char *get_packeg_command(Packeg *p);// return the command of the pkg
char *get_packeg_link(Packeg *p);// return the link
char *get_packeg_data(Packeg *p);
size_t get_packeg_data_size(Packeg *p);
size_t get_packeg_link_size(Packeg *p);

// creat the Datagram_store struct and return the pointer
Datagram_store *init_datagramstore();

// add bew datagram to the datagram_store struct
// calld in file mini_site function listin_server, when the function catch a msg from the client
int add_datagram(Datagram_store *datagram, char *data, int clientfd, int data_size); 

// extract the data and other information from the datagram.
Packeg *read_datagram(Datagram *dg);
// free the allocated mamory of Packeg
int free_datagram(Packeg *dg);

// clear the datagram store stract. used at the end
int clear_datagram_store(Datagram_store *datagram);
int free_datagram_store(Datagram_store *datagram);

// int generate_html_chat () : generate the html pyje;

#endif