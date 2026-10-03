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
                            HTML_page_store *html_data,
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

        int html_page_index = get_html_gape_by_link(html_data, pkg_link, pgk_link_size);
        char *html_page = NULL;
        if(html_page_index != 0){
            HTML_page *HP = get_html_page(html_data, html_page_index);
            int page_size = get_HP_size(HP);
            html_page = malloc(page_size);
            strcpy(html_page, get_HP_data(HP));

            size_t final_size = page_size + sizeof(MAIN_HADR);
            char *final_page = malloc(final_size);
            applay_haders(final_page, final_size, MAIN_HADR, final_page, page_size);

            if (send(client_fd, final_page, final_size, 0) == -1){perror("faild to send client a html page\n");}

            free(html_page);
            free(final_page);
        }
        
        if (strncmp(pkg_link, "/api/messages", pgk_link_size) == 0){
            char buff[8000];
            size_t send_len = 1; // add the thing

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

        char  *file_name_ptr = strcasestr(pkg_data, "file");
        file_name_ptr += 7;
        if(file_name_ptr != NULL){            
            char buff[100];
            int i = 0;
            while(true){
                if (*file_name_ptr == '\"'){break;}
                buff[i] = *file_name_ptr++;
                i++;
            }

            update_file(myfiles, buff, pkg_data, pkg_data_size, 2);
        }


        send(client_fd, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);
        global_sand(client_sock, pkg_data, pkg_data_size);
    }
}