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

    return 0;
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