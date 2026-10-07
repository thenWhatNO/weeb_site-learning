#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>

#include "server.h"

struct Server_socket{
    int fd; //soket number
    char *ip;
    struct addrinfo *server_info;
    int status; // 1 running, 0 off
};


Server_socket *initilize_server(char *ip, char *port){
    Server_socket *server_sock = malloc(sizeof(Server_socket));

    server_sock->ip = malloc(strlen(ip)+1);
    if (server_sock->ip == NULL){
        return NULL;
    }

    strcpy(server_sock->ip, ip);

    server_sock->fd = 0;
    server_sock->server_info = NULL;
    
    int err;
    
    struct addrinfo hint, *res;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    
    if ((err = getaddrinfo(ip, port, &hint, &res)) == -1){
        perror("faild at \"getaddrinfo()\"\n");
        return NULL;
    }
    
    for (struct addrinfo *p = res; p != NULL; p = p->ai_next){
        if ((err = server_sock->fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
            perror("faile at \"socket()\"\n");
            continue;
        }
        
        if((err = bind(server_sock->fd, p->ai_addr, p->ai_addrlen)) == -1){
            perror("falid at bind()\n");
            close(server_sock->fd);
            continue;
        }
        
        server_sock->server_info = malloc(sizeof(p)+1);
        server_sock->server_info = p;

        break;
    }

    freeaddrinfo(res);

    if (server_sock->server_info == NULL){
        if(server_sock->fd == 0){
            perror("false to create a socket\n");
        } else {
            perror("filed to binde the sever\n");
            close(server_sock->fd);
        }
        return NULL;
    }

    return server_sock;
}

int free_server(Server_socket *server_fd){
    free(server_fd->ip);
    server_fd->ip = NULL;

    free(server_fd->server_info);
    server_fd->server_info = NULL;

    free(server_fd);
    server_fd = NULL;

    return 0;
}

int clear_server_data(Server_socket *Server_socket){
    free(Server_socket->ip);
    freeaddrinfo(Server_socket->server_info);

    return 0;
}


int start_server(Server_socket *server_sock){
    int err;
    
    if((err = listen(server_sock->fd, 10)) == -1){
        perror("faild to start listening\n");
        return -1;
    }
    server_sock->status = 1;
    return 0;
}

int stop_server(Server_socket *server_sock){
    int err;
    
    if((err = shutdown(server_sock->fd, SHUT_RD)) < 0){
        perror("failde at shutdown\n");
        return -1;
    }

    return 0;
}

int accept_server(  Server_socket *server_sock, 
                    int timeout)
    {
    struct sockaddr_in new_client_sock;
    socklen_t client_sock_len = sizeof(new_client_sock);

    fd_set accaption;
    FD_ZERO(&accaption);
    FD_SET(server_sock->fd, &accaption);

    struct timeval tv = {timeout,0};
    int res = select(server_sock->fd+1, &accaption, NULL, NULL, &tv);
    if(res == 0){return 0;}
    else if(res == -1){
        perror("faild at accaption select\n");
        return -1;
    }
    else {
        int new_client = accept(server_sock->fd, (struct sockaddr*) &new_client_sock, &client_sock_len);
        if(new_client == -1){
            perror("faild at accation accapt\n");
            return -1;
        }

        return new_client;
    }

    return 0;
}

int close_server(Server_socket *server_sock){
    close(server_sock->fd);
    return 0;
}

