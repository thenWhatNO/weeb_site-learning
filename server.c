#include "server.h"
#include <string.h>


int initilize_server(char *ip, char *port, Server_socket *server_sock){

    server_sock->ip = malloc(strlen(ip)+1);
    if (server_sock->ip == NULL){
        return -1;
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
        return -1;
    }
    
    for (struct addrinfo *p = res; p != NULL; p = p->ai_next){
        if ((err = server_sock->fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
            perror("faile at \"socket()\"\n");
            continue;
        }
        
        if(err = bind(server_sock->fd, p->ai_addr, p->ai_addrlen) == -1){
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
        return -1;
    }

    return 1;
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
                    Clients_socket *client_sock, 
                    size_t timeout)
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

        add_client(client_sock, new_client);
    }

    return 0;
}


int listin_server(  Datagram_store *DTgrams,  // this sould be preperd before useg.
                    Server_socket *server_sock ,
                    Clients_socket *client_socket,  
                    size_t timeout)
    {
    
    int i;

    fd_set clients;
    FD_ZERO(&clients);

    for (i = 0; i < MAX_CLIENTS; i++){
        if(client_socket[i].is_cl == 3){
            FD_SET(i, &clients);
        }
    }

    struct timeval tv = {timeout,0};

    int last = last_open_socket(client_socket);
    
    int res = select(last+1, &clients, NULL, NULL, &tv);
    if(res == 0){return 0;}
    else if(res == -1){
        perror("faild to select sesrver listion\n");
        return -1;
    }

    char buff[1024];
    memset(buff, 0, 1024);

    for(i = 0; i < MAX_CLIENTS; i++){

        if(client_socket[i].is_cl != 3){continue;}

        if(FD_ISSET(i, &clients) == 0){
            continue;
        }

        int get = recv(i, buff, 1024, 0);
        
        if(get == -1) {continue;}
        else if (get > 0){
            add_datagram(DTgrams, buff, i, get);
        }
        
        memset(buff, 0, 1024);
    }
    return 0;
}

int send_datagram(  HTML_datagram *html_data,
                    Clients_socket *client_socket,
                    char *others
                )
    {

    if (html_data->len >= 9999){
        perror("the msg size of bigger then 9999\n");
        return -1;
    }

    int get;

    for (int i = 0; i < MAX_CLIENTS; i++){
        
        if(client_socket[i].is_cl != 3){continue;}
        
        size_t others_len = strlen(others);
        if(others_len > 0){
            if(strncmp(client_socket[i].command , "/api/stream", 11) != 0){continue;}
            char frame[1024];
            int len = snprintf(frame, sizeof(frame),
                            "data: %s\n\n",
                            others);
            
            get = send(i, frame, len, MSG_NOSIGNAL);
            continue;
        }

        if(html_data->updated){
            get = send(i, html_data->html_msg, html_data->len, 0);
            continue;
        }
        
        if(!view_client_status(client_socket, i)){
            remove_client(client_socket, i);
        }
    }
    html_data->updated = 0;
    return 0;
}

int close_server(Server_socket *server_sock){
    close(server_sock->fd);
    return 0;
}

