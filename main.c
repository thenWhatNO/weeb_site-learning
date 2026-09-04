#include "server.h"

int main(){

    Files_struct my_files;
    open_files(&my_files);

    Server_socket my_server;
    if (initilize_server("192.168.0.114", "8080", &my_server) == -1){
        perror("coulend initialize server sockets\n");
        return -1;
    }

    Client_socket my_clients;
    if (init_client_sock(& my_clients) == -1){
        perror("coulend initialize clients sockets\n");
        return -1;
    }

    Datagram_store my_datagrams;
    init_datagramstore(&my_datagrams);

    start_server(&my_server);

    printf("server up on port : 8080\n");
    
    HTML_datagram html_struct;
    init_html_struct(&html_struct);

    while(1){
        accept_server(&my_server, &my_clients, 1);

        listin_server(&my_datagrams, &my_server, &my_clients, 1);
        
        read_client_msg(&my_datagrams, &html_struct, &my_files);

        send_datagram(&my_server, &html_struct, &my_clients);
        clear_datagram(&my_datagrams);
    }
    free_html_stract(&html_struct);

    stop_server(&my_server);

    close_files(&my_files);
    clear_client_socket(&my_clients);
    clear_datagram(&my_datagrams);
    close_server(&my_server);

    return 0;
}