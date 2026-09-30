#include "mini_site.h"

int main(){

    Server_socket *my_server = initilize_server("192.168.0.114", "8080");
    if (my_server == NULL){
        perror("coulend initialize server sockets\n");
        return -1;
    }

    Clients_socket *my_clients = init_client_sock();

    HTML_page_store *HPS = init_html_struct();
    add_html_page(HPS, "main", 4, "/main-page", 10, "");
    printf("loaded html_pages");

    Datagram_store *datagrams = init_datagramstore();

    start_server(my_server);
    printf("server up on port : 8080\n");

    int get;
    while(1){
        if((get = accept_server(my_server, 1)) > 0){add_client(my_clients, get);}

        listin_server(datagrams, my_server, my_clients, 1);
        
        read_client_msg(&my_datagrams, &html_struct, &my_files, my_clients);

        clear_datagram(&my_datagrams);
    }
    free_html_page_store(&HPS);

    stop_server(my_server);

    clear_client_socket(my_clients);
    clear_datagram(&my_datagrams);
    close_server(my_server);

    return 0;
}