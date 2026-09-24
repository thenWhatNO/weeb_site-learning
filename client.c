#include <stdio.h>
#include <string.h>
#include "client.h"

typedef struct {
    char command[100];
    int active;
    int is_cl;
} Clients_socket;

Clients_socket *init_client_sock(){

    Clients_socket *client_sock = malloc(sizeof(Clients_socket));

    for(int i = 0; i < MAX_CLIENTS; i++){
        client_sock[i].active = 0;
        client_sock[i].is_cl = 0;
        memset(client_sock[i].command, 0, 100); 
    }
    
    return client_sock;
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


// this program just sand the same data into every open client fd
int global_sand(    Clients_socket *client_sock,
                    char *data,
                    size_t data_l
                )
    {

    int get;

    int faild_send[MAX_CLIENTS];
    int pointer = 0;

    int i;
    for (i = 3; i < MAX_CLIENTS; i++){
        if(client_sock[i].is_cl){
            get = send(i, data, data_l, MSG_NOSIGNAL);
            if(get == -1){
                faild_send[pointer++] = i;
            }
        }
    }

    for (i = 0; i < MAX_CLIENTS; i++){
        get = send(faild_send[i], data, data_l, MSG_NOSIGNAL);
    }

    return 0;
}

int view_clients_status(Clients_socket *client_sock){
    system("clear");
    printf("fd\t\tstatus\t\tcomannd\n");
    for(int i = 0; i < MAX_CLIENTS; i++){
        printf("%d\t\t%d\t\t%s\n", i, client_sock[i].is_cl, client_sock[i].command);
    }
    print("pres Q to stop\n");
    return 0;
}