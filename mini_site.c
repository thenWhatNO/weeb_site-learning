#include "mini_site.h"

int listin_server(  Datagram_store *DTgrams,  // this sould be preperd before useg.
                    Server_socket *server_sock ,
                    Clients_socket *client_socket,  
                    size_t timeout)
    {
    
    int i;

    fd_set clients;
    FD_ZERO(&clients);

    for (i = 0; i < MAX_CLIENTS; i++){
        if(is_client_true(client_socket, i)){
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

        if(!is_client_true(client_socket, i)){continue;}

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

int read_client_msg_copy(   Packeg *pkg,
                            char *html_data, // can bee change to char data type
                            size_t html_data_l,
                            Files_struct *myfiles,
                            Clients_socket *client_sock,
                            int client_fd
                )
    {

    int get;

    char *pkg_command = get_packeg_command(pkg); 

    if(strcmp(pkg_command, "GET") == 0){
        char *pkg_link = get_packeg_link(pkg);
        size_t pgk_link_size = get_packeg_link_size(pkg);

        if (strncmp(pkg_link, "/", pgk_link_size) == 0){

            if (html_data_l >= 9999){
                perror("the msg size of bigger then 9999\n");
                return -1;
            }
            
            get = send(client_fd, html_data, html_data_l, 0);
            if (get <= 0){
                perror("faild to ansar to GET request in read function\n");
            }
        } 
        else if (strncmp(pkg_link, "/api/messages", pgk_link_size) == 0){
            char buff[8000];
            size_t send_len = generate_msg(myfiles, buff);

            get = send(client_fd, buff, send_len, 0);
            if (get <= 0){
                perror("faild to ansar to GET request in read function\n");
            }
        }
        else if(strncmp(pkg_link, "/api/stream", pgk_link_size) == 0){
            char headers[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/event-stream\r\n"
            "Cache-Control: no-cache\r\n"
            "Connection: keep-alive\r\n"
            "\r\n";

            send(client_fd, headers, strlen(headers), 0);
        }
        else {
            get = send(client_fd, ALIVE_HADR, strlen(ALIVE_HADR), 0);
            if (get <= 0){
                perror("faild to ansar to GET request in read function\n");
            }
        }

        return 0;
    }
    
    if(strcmp(pkg_command, "POST") == 0){

        char *pkg_data = get_packeg_data(pkg);
        size_t pkg_data_size = get_packeg_data_size(pkg);

        if(pkg_data){
            const char *resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
            printf("sending error msg to client %d", client_fd);
            send(client_fd, resp, strlen(resp), 0);
            remove_client(client_sock, client_fd);
            return 0;
        }

        char mesg[100];
        char time[100];
        char name[100];

        sscanf(pkg_data, "{\"name\":\"%[^\"]\",\"text\":\"%[^\"]\",\"time\":\"%10[^\"]\"}", name, mesg, time);

        size_t client_data_len = strlen(mesg) + 14;
        char *client_data = malloc(client_data_len);
        sprintf(client_data, "%s: %s [%s]\n", name, mesg, time);

        size_t w_data_l = strlen(name) + strlen(mesg) + strlen(time) + 35;
        char *w_data = malloc(w_data_l);
        sprintf(w_data, ",\n{\"name\":\"%s\",\"text\":\"%s\",\"time\":\"%s\"}\n]", name, mesg, time);

        update_chat_file(myfiles, w_data, w_data_l);

        send(client_fd, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);
        global_sand(client_sock, pkg_data, pkg_data_size);
        
        free(w_data);
        free(client_data);

    }
}