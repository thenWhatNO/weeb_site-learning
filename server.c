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

int clear_datagram(Datagram_store *Dgram){
    for (int i = 0; i < Dgram->size; i++){
        memset(Dgram->datagrams[i].msg, 0, Dgram->datagrams[i].msg_size);
        Dgram->datagrams[i].msg_size = 0;
        Dgram->datagrams[i].client_id = 0;
    }

    Dgram->size = 0;

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
        else if (get == 0){
            // FD_CLR(cor_client, &clients);
            // remove_client(client_socket, cor_client, client_socket->size);
        }
        else if (get > 0){
            add_datagram(DTgrams, buff, i, get);
        }
        
        memset(buff, 0, 1024);
    }
    return 0;
}

int send_datagram(  Server_socket *server_sock,
                    HTML_datagram *html_data,
                    Clients_socket *client_socket
                )
    {

    if (html_data->len >= 9999){
        perror("the msg size of bigger then 9999\n");
        return -1;
    }

    int get;

    for (int i = 0; i < MAX_CLIENTS; i++){

        if(client_socket[i].is_cl != 3){continue;}

        if(html_data->updated){
            get = send(i, html_data->html_msg, html_data->len, 0);
        }

        get = send(i, ": ping\n\n", 9, MSG_NOSIGNAL);

        if(get == -1){
            remove_client(client_socket, i);
            return -1;
        }
    }
    html_data->updated = 0;
    return 0;
}

int close_server(Server_socket *server_sock){
    close(server_sock->fd);
    return 0;
}

int read_client_msg(Datagram_store *DTgrams, 
                    HTML_datagram *html_data, 
                    Files_struct *myfiles
                )
    {
    if (DTgrams->size <= 0){return 0;}

    
    for (int i = 0; i < DTgrams->size; i++){
        //printf("msg from : %d\n %s\n\n", DTgrams->datagrams[i].client_id, DTgrams->datagrams[i].msg);


        if(strlen(DTgrams->datagrams[i].msg) <= 0){continue;} //chack that there IS a msg

        char *HADER_C = strcasestr(DTgrams->datagrams[i].msg, "GET"); // check if its a GET
        if(HADER_C != NULL){

            // need to add the fitration for a page that the client want ot reach.

            char requst[100];
            sscanf(DTgrams->datagrams[i].msg, "GET %s", requst);
            size_t rqst_len = strlen(requst);

            int get;

            if (strncmp(requst, "/", rqst_len) == 0){
                generate_data(html_data, myfiles, DTgrams, MAIN_HADR);
    
                if (html_data->len >= 4096){
                    perror("the msg size of bigger then 4096\n");
                    return -1;
                }
    
                get = send(DTgrams->datagrams[i].client_id, html_data->html_msg, html_data->len, 0);
                if (get <= 0){
                    perror("faild to ansar to GET request in read function\n");
                }
            } 
            else if (strncmp(requst, "/api/messages", rqst_len) == 0){
                char *buff = malloc(1);
                size_t send_len = generate_msg(myfiles, &buff);

                printf("%s\n", buff);
                get = send(DTgrams->datagrams[i].client_id, buff, send_len, 0);
                if (get <= 0){
                    perror("faild to ansar to GET request in read function\n");
                }
            }
            else {
                get = send(DTgrams->datagrams[i].client_id, ALIVE_HADR, strlen(ALIVE_HADR), 0);
                if (get <= 0){
                    perror("faild to ansar to GET request in read function\n");
                }
            }

            continue;
        }
        


        HADER_C = strcasestr(DTgrams->datagrams[i].msg, "POST"); // check if it a POST, and do the rest
        if(HADER_C == NULL){continue;}
        else {
            char *cl = strcasestr(DTgrams->datagrams[i].msg, "Content-Length:");

            if(cl == NULL){
                const char *resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
                printf("sending error msg to client %d", DTgrams->datagrams[i].client_id);
                send(DTgrams->datagrams[i].client_id, resp, strlen(resp), 0);
                //remove_client()
                return -1;
            }

            int content_Length = atoi(cl+15);

            char *content = calloc(content_Length+1, sizeof(char));
            if(content == NULL){return -1;}

            int start = DTgrams->datagrams[i].msg_size - content_Length;
            strcpy(content, DTgrams->datagrams[i].msg + start);
            content[content_Length+1] = '\0';

            char mesg[100];
            char time[100];
            char name[100];

                          //"{\"name\":\"%[^\"]\",\"text\":\"%[^\"]\",\"time\":\"%10[^\"]\"}"
            sscanf(content, "{\"name\":\"%[^\"]\",\"text\":\"%[^\"]\",\"time\":\"%10[^\"]\"}", name, mesg, time);

            size_t client_data_len = strlen(mesg) + 14;
            char *client_data = calloc(client_data_len, sizeof(char));
            sprintf(client_data, "%s: %s [%s]\n", name, mesg, time);

            size_t w_data_l = strlen(name) + strlen(mesg) + strlen(time) + 35;
            char *w_data = malloc(w_data_l);
            sprintf(w_data, ",\n{\"name\":\"%s\",\"text\":\"%s\",\"time\":\"%s\"}\n]", name, mesg, time);
            
            printf("%s\n", w_data);

            update_chat_file(myfiles, w_data, w_data_l);

            free(w_data);
            free(content);
            free(client_data);

            generate_data(html_data, myfiles, DTgrams, MAIN_HADR);
            html_data->updated = 1;

        }

    }
}

int generate_data(  HTML_datagram *html_data, 
                    Files_struct *myfiles, 
                    Datagram_store *DTgrams,
                    char *hadr
                )
    {
    
    char *html_tamply = malloc(1);
    size_t html_len;

    html_len = read_file(myfiles, 'm', &html_tamply);

    size_t total_len = html_len + strlen(hadr);
    char data_buff[total_len];
    
    snprintf(data_buff, total_len, hadr, html_len, html_tamply);

    write_msg_to_html(html_data, data_buff, total_len);

    free(html_tamply);
    return total_len;
}

int generate_msg(Files_struct *myfiles, char **buff){
    char *chat_data = malloc(1);
    size_t chat_len;

    chat_len = read_file(myfiles, 'c', &chat_data);
    size_t total_size = chat_len + strlen(MAEG_HADR);
    
    if(*buff == NULL){
        *buff = malloc(total_size);
    } else if (strlen(*buff) < total_size){
        *buff = realloc(*buff, total_size);
    }

    snprintf(*buff, total_size, MAEG_HADR, chat_len, chat_data);

    return total_size;
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

int open_files(Files_struct *myfiles){
    myfiles->chat_file = fopen("html_files/chat_block.json", "r+");
    if (myfiles->chat_file == NULL){
        perror("cant open chat file. maybe not exist\n");
        return -1;
    }

    myfiles->html_file = fopen("html_files/main-page.html", "r+");
    if (myfiles->html_file == NULL){
        perror("cant open html page file. maybe not exist\n");
        fclose(myfiles->chat_file);
        return -1;
    }

    return 0;
}

int close_files(Files_struct *myfiles){
    fclose(myfiles->chat_file);
    fclose(myfiles->html_file);

    return 0;
}

int update_chat_file(Files_struct *myfiles, char *data, size_t d_size){
    if(d_size > 9999){
        perror("data size too big to save in the file\n");
        return -1;
    }

    fseek(myfiles->chat_file, -2, SEEK_END);
    if(fwrite(data, 1, d_size, myfiles->chat_file) != d_size){
        perror("could not write data into chat file\n");
        rewind(myfiles->chat_file);
        return -1;
    }
    rewind(myfiles->chat_file);
    return 0;
}

int read_file(Files_struct *myfiles, char file, char **buffer){
    FILE *ptr = NULL;
    if (file == 'm'){
        ptr = myfiles->html_file;
    }
    if(file == 'c'){
        ptr = myfiles->chat_file;
    }
    
    if(ptr == NULL){
        return -1;
    }

    fseek(ptr, 0, SEEK_END);
    long file_size = ftell(ptr);
    rewind(ptr);

    if(*buffer == NULL){
        *buffer = malloc((file_size+1) * sizeof(char));
    }
    if(strlen(*buffer) < file_size){
        *buffer = realloc(*buffer, (file_size+1) * sizeof(char));
    }

    size_t full_l = fread(*buffer, 1, file_size, ptr);
    (*buffer)[full_l] = '\0';

    return full_l;
}