#include <stdio.h>
#include "client.h"


int init_client_sock(Client_socket *client_sock){
    client_sock->cap = 10;
    client_sock->size = 0;
    client_sock->fds = calloc(client_sock->cap, sizeof(int));
    client_sock->ides = calloc(client_sock->cap, sizeof(int));

    return 0;
}

int add_client(Client_socket *client_sock,int fd, int id){
    if(client_sock->size >= client_sock->cap){
        client_sock->cap += 10;
        client_sock->fds = realloc(client_sock->fds, sizeof(int)*client_sock->cap);
        client_sock->ides = realloc(client_sock->ides, sizeof(int)*client_sock->cap);
    }

    client_sock->fds[client_sock->size] = fd;
    client_sock->ides[client_sock->size] = id;

    client_sock->size++;

    return 0;
}

int remove_client(Client_socket *client_sock, int fd, int id){
    int cleard = 0;
    for (int i = 0; i < client_sock->size; i++){
        
        if(client_sock->fds[i] == fd){
            client_sock->fds[i] = 0;
            client_sock->ides[i] = 0;
            cleard = 1;
        }

        if(cleard){
            client_sock->fds[i] = client_sock->fds[i+1];
            client_sock->ides[i] = client_sock->ides[i+1];
        }
        
    }

    printf("remove client : %d\n", fd);

    if(cleard){
        client_sock->size--;
        return 0;
    }
    else{return -1;}
}

int clear_client_socket(Client_socket *client_sock){
    free(client_sock->fds);
    free(client_sock->ides);

    return 0;
}

int update_clients_fd(Client_socket *client_sock){
    // update the clients fd acurding to there id
}