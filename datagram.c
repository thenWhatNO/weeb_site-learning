#include "datagram.h"
#include "server.h"
#include "html_data.h"

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

int read_client_msg(Datagram_store *DTgrams, 
                    HTML_datagram *html_data, 
                    Files_struct *myfiles,
                    Clients_socket *client_sock
                )
    {
    if (DTgrams->size <= 0){return 0;}

    for (int i = 0; i < DTgrams->size; i++){

        if(strlen(DTgrams->datagrams[i].msg) <= 0){continue;} //chack that there IS a msg

        char *HADER_C = strcasestr(DTgrams->datagrams[i].msg, "GET"); // check if its a GET
        if(HADER_C != NULL){

            char requst[100];
            sscanf(DTgrams->datagrams[i].msg, "GET %s", requst);
            size_t rqst_len = strlen(requst);

            int get;
            
            strcpy(client_sock[DTgrams->datagrams[i].client_id].command, requst);

            if (strncmp(requst, "/", rqst_len) == 0){
                generate_data(html_data, myfiles, DTgrams, MAIN_HADR);
    
                if (html_data->len >= 9999){
                    perror("the msg size of bigger then 9999\n");
                    return -1;
                }
                
                get = send(DTgrams->datagrams[i].client_id, html_data->html_msg, html_data->len, 0);
                if (get <= 0){
                    perror("faild to ansar to GET request in read function\n");
                }
            } 
            else if (strncmp(requst, "/api/messages", rqst_len) == 0){
                char buff[8000];
                size_t send_len = generate_msg(myfiles, buff);

                get = send(DTgrams->datagrams[i].client_id, buff, send_len, 0);
                if (get <= 0){
                    perror("faild to ansar to GET request in read function\n");
                }
            }
            else if(strncmp(requst, "/api/stream", rqst_len) == 0){
                char headers[] =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/event-stream\r\n"
                "Cache-Control: no-cache\r\n"
                "Connection: keep-alive\r\n"
                "\r\n";

                send(DTgrams->datagrams[i].client_id, headers, strlen(headers), 0);
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
                remove_client(client_sock, DTgrams->datagrams[i].client_id);
                continue;
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

            sscanf(content, "{\"name\":\"%[^\"]\",\"text\":\"%[^\"]\",\"time\":\"%10[^\"]\"}", name, mesg, time);

            size_t client_data_len = strlen(mesg) + 14;
            char *client_data = calloc(client_data_len, sizeof(char));
            sprintf(client_data, "%s: %s [%s]\n", name, mesg, time);

            size_t w_data_l = strlen(name) + strlen(mesg) + strlen(time) + 35;
            char *w_data = malloc(w_data_l);
            sprintf(w_data, ",\n{\"name\":\"%s\",\"text\":\"%s\",\"time\":\"%s\"}\n]", name, mesg, time);

            update_chat_file(myfiles, w_data, w_data_l);

            send(DTgrams->datagrams[i].client_id, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);
            send_datagram(html_data, client_sock, content);
            
            free(w_data);
            free(content);
            free(client_data);

            generate_data(html_data, myfiles, DTgrams, MAIN_HADR);
            html_data->updated = 1;

        }

    }
}