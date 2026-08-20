#include "server.h"
#include <string.h>

int init_datagramstore(Datagram_store *datagram){
    datagram->cap = 10;
    datagram->size = 0;

    for (int i = 0; i < datagram->cap; i++){
        datagram->datagrams = malloc(sizeof(Datagram) * datagram->cap);
    }

    return 0;
}

int add_datagram(Datagram_store *datagram, char *data, int clientfd){

    if (datagram->size >= datagram->cap){
        datagram->cap += 10;
        datagram->datagrams = realloc(datagram->datagrams, datagram->cap);
    }

    datagram->datagrams[datagram->size].client_id = clientfd;
    datagram->datagrams[datagram->size].msg = malloc(strlen(data)+1);
    strcpy(datagram->datagrams[datagram->size].msg, data); 

    datagram->size++;
    return 0;
}

int clear_datagram(Datagram_store *datagram){
    for (int i = 0; i < datagram->size; i++){
        free(datagram->datagrams[i].msg);
    }

    free(datagram->datagrams);

    return 0;
}

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
                    Client_socket *client_sock, 
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

        add_client(client_sock, new_client, new_client);
    }

    return 0;
}


int listin_server(  Datagram_store *DTgrams,  // this sould be preperd before useg.
                    Server_socket *server_sock ,
                    Client_socket *client_socket,  
                    size_t timeout)
    {

    if (client_socket->size <= 0){return 0;}
    
    int i;

    fd_set clients;
    FD_ZERO(&clients);

    FD_SET(server_sock->fd, &clients);

    for (i = 0; i <= client_socket->size; i++){
        FD_SET(client_socket->fds[i], &clients);
    }

    struct timeval tv = {timeout,0};
    
    int res = select(client_socket->fds[client_socket->size-1]+1, &clients, NULL, NULL, &tv);
    if(res == 0){return 0;}
    else if(res == -1){
        perror("faild to select sesrver listion\n");
        return -1;
    }

    char buff[1024];

    for(i = 0; i <= client_socket->size; i++){
        int cor_client = client_socket->fds[i];
        if(FD_ISSET(cor_client, &clients) == 0){
            continue;
        }

        int get = recv(cor_client, buff, sizeof(buff), 0);
        
        if(get == -1) {continue;}
        else if (get == 0){
            FD_CLR(cor_client, &clients);
            
            printf("client %d disconect\n", cor_client);
            
            remove_client(client_socket, cor_client, client_socket->size);
        }
        
        //printf("%s\n", buff);
        // todo: need to add a new function that well filter commands.
        //add_datagram(DTgrams, buff, cor_client);
        memset(buff, 0, sizeof(buff));
    }
    return 0;
}

int send_datagram(  Server_socket *server_sock,
                    HTML_datagram *html_data,
                    Client_socket *client_socket,
                    size_t dg_size)
    {
    if (client_socket->size <= 0){return 0;}

    if (html_data->len >= 4096){
        perror("the msg size of bigger then 4096\n");
        return -1;
    }

    int get;

    for (int i = 0; i < client_socket->size; i++){
        get = send(client_socket->fds[i], html_data->html_msg, html_data->len, 0);

        if(get == -1){
            printf("faild to send data to client %d\n", client_socket->fds[i]);
        }
    }

    return 0;
}

int close_server(Server_socket *server_sock){
    close(server_sock->fd);
    return 0;
}

int generate_data(HTML_datagram *html_data, size_t data_len, Datagram_store *DTgrams){
    
    const char *html_haders = "HTTP/1.0 200 OK\r\n"
                              "Content-Type: text/html; charset=UTF-8\r\n"
                              "Content-Length: %zu\r\n"
                              "Connection: Close\r\n"
                              "Cookie: name=123\r\n"
                              "\r\n"
                              "%s";
    
    char *html_tamply;
    size_t html_len;

    FILE *ptr = fopen("html_files/main-page.html", "r");
    if(ptr == NULL){return -1;}

    fseek(ptr, 0, SEEK_END);
    long full_len = ftell(ptr);
    rewind(ptr);

    html_tamply = malloc(sizeof(char) + full_len + 1);
    html_len = fread(html_tamply, 1, full_len, ptr);
    html_tamply[html_len] = '\0';
    fclose(ptr);


    char *html = NULL;
    char *chat = NULL;


    if (DTgrams->size > 0){
        int i;
        
        size_t chat_len = 1;

        for (i = 0; i < DTgrams->size; i++){
            chat_len += strlen(DTgrams->datagrams[i].msg);
            chat_len += 2;
        }

        chat = malloc(chat_len);
        if (chat == NULL) return -1;
        chat[0] = '\0';

        for (i = 0; i < DTgrams->size; i++){
            strcat(chat, DTgrams->datagrams[i].msg);
            strcat(chat, "\n");
        }
        html_len = strlen(html_tamply) + strlen(chat) + 1;
        html = malloc(html_len);
        if (html == NULL) {
            free(chat);
            return -1;
        }
        snprintf(html, html_len, html_tamply, chat);
    } else {
        char *non = "there nothing to show\n";
        html_len = strlen(html_tamply) + strlen(non);
        html = malloc(html_len);
        if (html == NULL) {return -1;}

        snprintf(html, html_len, html_tamply, non);
    }

    size_t total_len = html_len + strlen(html_haders);
    char data_buff[total_len];
    
    char ret = snprintf(data_buff, total_len, html_haders, html_len, html);

    write_msg_to_html(html_data, data_buff, total_len);

    if(html != NULL){free(html);}
    if(chat != NULL){free(chat);}

    return 1;
}

int init_html_struct(HTML_datagram *html_str){
    html_str->cap = 10;
    html_str->len = 0;
    html_str->html_msg = calloc(html_str->cap, sizeof(char));

    if(html_str->html_msg == NULL){return -1;}

    return 0;
}

int write_msg_to_html(HTML_datagram *html_data, char *msg, size_t msg_len){
    if(html_data->cap < msg_len){
        html_data->cap = msg_len;
        html_data->html_msg = realloc(html_data->html_msg, html_data->cap);
    }

    html_data->len = msg_len;

    if (html_data->html_msg == NULL){return -1;}
    memset(html_data->html_msg, 0, html_data->cap);
    strcpy(html_data->html_msg, msg);

    return 0;
}

int free_html_stract(HTML_datagram *html_data){
    free(html_data->html_msg);
    html_data->html_msg = NULL;

    return 0;
}