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

char *get_packeg_command(Packeg *p);
char *get_packeg_link(Packeg *p);
char *get_packeg_data(Packeg *p);
size_t get_packeg_data_size(Packeg *p);
size_t get_packeg_link_size(Packeg *p);

// int generate_html_chat () : generate the html pyje;

#endif