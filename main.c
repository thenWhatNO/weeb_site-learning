#include "mini_site.h"

int main(){
    Datagram_store *datagrams = init_datagramstore();
    
    HTML_page_store *HPS = init_html_struct();
    add_html_page(HPS, "main", 4, "/", 10, "html_files/main-page.html");
    printf("loaded html_pages\n");
    
    Clients_socket *my_clients = init_client_sock();
    
    Files_struct *my_F = init_File_struct();
    add_file(my_F, "chat", "html_files/chat_block.json");
    printf("loaded files\n");
    
    Server_socket *my_server = initilize_server("192.168.1.149", "8080");
    if (my_server == NULL){
        perror("coulend initialize server sockets\n");
        return -1;
    }
    start_server(my_server);
    printf("server up on port : 8080\n");
    
    int get;
    for (int ii = 0; ii < 10; ii++){
        if((get = accept_server(my_server, 1)) > 0) add_client(my_clients, get);

        listin_server(datagrams, my_server, my_clients, 1);

        view_clients_status(my_clients);

        for (int i = 0; i < get_datagram_store_size(datagrams); i++){
            Datagram *corent_dg = get_datagram_store_datagram(datagrams, i);

            Packeg *pkg = read_datagram(corent_dg);
            int client_fd = get_datagram_client_fd(corent_dg);

            read_client_msg_copy(pkg, HPS, my_F, my_clients, client_fd);
            
            free_datagram(pkg);
            pkg = NULL;
            corent_dg = NULL;
        }

        clear_datagram_store(datagrams);
    }

    free_file(my_F);
    free_html_page_store(HPS);
    free_datagram_store(datagrams);

    free(my_clients);
    my_clients = NULL;

    stop_server(my_server);
    close_server(my_server);

    free_server(my_server);

    return 0;
}