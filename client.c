#include <stdio.h>
#include <string.h>
#include "client.h"

int init_client_sock(Clients_socket *client_sock){
    for(int i = 0; i < MAX_CLIENTS; i++){
        client_sock[i].active = 0;
        client_sock[i].is_cl = 0;
        memset(client_sock[i].command, 0, 100); 
    }
    
    return 0;
}

int add_client(Clients_socket *client_sock, int fd){
    client_sock[fd].active = 1;
    client_sock[fd].is_cl = 3;

    return 0;
}

int add_command_to_client(Clients_socket *client_sock, int fd, char *command){
    strcpy(client_sock[fd].command, command);
    return 0;
}

int remove_client(Clients_socket *client_sock, int fd){
    client_sock[fd].active = 0;
    client_sock[fd].is_cl = 0;
    memset(client_sock[fd].command, 0, 100);

    close(fd);

    printf("close fd : %d\nactiv: %d\nis_cl: %d\ncommand: %s", fd, 
        client_sock[fd].active, client_sock[fd].is_cl, client_sock[fd].command);

    return 0;
}

int view_client_status(Clients_socket *client_sock, int fd){
        char c;
        ssize_t n = recv(fd, &c, 1, MSG_PEEK | MSG_DONTWAIT);
        if (n > 0)  return 1;              // client sent us something (unexpected on SSE, but alive)
        if (n == 0) return 0;              // peer performed orderly shutdown — CLOSED
        // n == -1:
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 1;                      // no data waiting, connection fine
        return -1;                         // ECONNRESET etc — dead
}

int last_open_socket(Clients_socket *client_sock){
    int lsteset = 0;

    for (int i = 0; i < MAX_CLIENTS; i++){
        if(client_sock[i].active == 1){
            lsteset = i;
        }
    }

    return lsteset;
}

int clear_client_socket(Clients_socket *client_sock){
    
    for (int i = 0; i < MAX_CLIENTS; i++){
        client_sock[i].active = 0;
        client_sock[i].is_cl = 0;
        memset(client_sock[i].command, 0, 100); 
    }

    return 0;
}

int view_clients_status(Clients_socket *client_sock){
    system("clear");
    printf("----------------------\n");
    for (int i = 0; i < MAX_CLIENTS; i++){
        printf("df : %d, command : %s\n", i, client_sock[i].command);
    }
    printf("----------------------\n");
    return 0;
}