#include <stddef.h>
#include "datagram.h"
#include "incldes_libs.h"

struct Datagram{
    char *msg;
    int client_id;
    int msg_size;
    //todo add time :3 NEVER!!
};

struct Datagram_store{
    int size;
    int cap;
    Datagram *datagrams;
};

struct Packeg{
    char command[18];
    size_t link_size;
    char *link;
    size_t data_size;
    char *data;
};

char *get_packeg_command(Packeg *p){return p->command;};
char *get_packeg_link(Packeg *p){return p->link;};
char *get_packeg_data(Packeg *p){return p->data;};
size_t get_packeg_link_size(Packeg *p){return p->link_size;};
size_t get_packeg_data_size(Packeg *p){return p->data_size;};

Datagram_store *init_datagramstore(){

    Datagram_store *datagram = malloc(sizeof(Datagram_store));
    
    datagram->cap = 10;
    datagram->size = 0;

    for (int i = 0; i < datagram->cap; i++){
        datagram->datagrams = malloc(sizeof(Datagram) * datagram->cap);
    }

    return datagram;
}

int add_datagram(Datagram_store *datagram, char *data, int clientfd, int data_size){

    if (datagram->size >= datagram->cap){
        datagram->cap += 10;
        datagram->datagrams = realloc(datagram->datagrams, datagram->cap);
    }

    datagram->datagrams[datagram->size].client_id = clientfd;
    datagram->datagrams[datagram->size].msg_size = data_size;
    datagram->datagrams[datagram->size].msg = malloc(data_size+1);
    strcpy(datagram->datagrams[datagram->size].msg, data); 
    datagram->datagrams[datagram->size].msg[data_size] = '\0';

    datagram->size++;
    return 0;
}

int free_datagram_store(Datagram_store *Dgram){
    for (int i = 0; i < Dgram->size; i++){
        memset(Dgram->datagrams[i].msg, 0, Dgram->datagrams[i].msg_size);
        Dgram->datagrams[i].msg_size = 0;
        Dgram->datagrams[i].client_id = 0;
    }

    Dgram->size = 0;

    return 0;
}

Packeg *read_datagram(Datagram *dg){

    Packeg *pkg = malloc(sizeof(Packeg));
    if(pkg == NULL) return NULL;

    char *HADER_C = strcasestr(dg->msg, "GET");
    if(HADER_C != NULL){
        strcpy(pkg->command, "GET");
        
        char requst[100];
        sscanf(dg->msg, "GET %s", requst);
        size_t rqst_len = strlen(requst);

        pkg->link_size = rqst_len;

        pkg->link = malloc(rqst_len);
        if(pkg->link == NULL){
            free(pkg);
            return NULL;
        }

        strcpy(pkg->link, requst);

        return pkg;
    }

    HADER_C = strcasestr(dg->msg, "POST");
    if(HADER_C != NULL){
        strcpy(pkg->command, "POST");

        char *cl = strcasestr(dg->msg, "Content-Length:");
        pkg->data_size = atoi(cl+15);
        pkg->data = malloc(pkg->data_size);

        if(pkg->data == NULL){
            free(pkg);
            return NULL;
        }

        int start = dg->msg_size - pkg->data_size;
        strcpy(pkg->data,dg->msg + start);
        pkg->data,dg->msg[pkg->data_size+1] = '\0';
    }
}

int free_datagram(Packeg *dg){
    if(dg->data != NULL){
        free(dg->data);
        dg->data = NULL;
    }
    if(dg->link != NULL){
        free(dg->link);
        dg->link = NULL;
    }

    if(dg->data != NULL || dg->link != NULL)return -1;

    return 1;
}