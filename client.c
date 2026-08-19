#include "client.h"


int init_client_sock(Client_socket *client_sock){
    client_sock->cap = 10;
    client_sock->size = 0;
    client_sock->fds = malloc(sizeof(int) * client_sock->cap);
    client_sock->ides = malloc(sizeof(int) * client_sock->cap);

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
    // not shure illl neeed to do that
    /// seans im gonna clear the clients list evry time for new client connectiom
}

int clear_client_socket(Client_socket *client_sock){
    free(client_sock->fds);
    free(client_sock->ides);

    return 0;
}

int update_clients_fd(Client_socket *client_sock){
    // update the clients fd acurding to there id
}