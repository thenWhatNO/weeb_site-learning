#ifndef MINI_SITE_H
#define MINI_SITE_H

#include "incldes_libs.h"
#include "client.h"
#include "server.h"
#include "datagram.h"
#include "files_m.h"
#include "html_data.h"
#include "mini_site.h"

int listin_server(Datagram_store *DTgrams, Server_socket *server_fd ,Clients_socket *client_socket , size_t timeout);
int read_client_msg_copy(Packeg *pkg, HTML_page_store *html_data, Files_struct *myfiles, Clients_socket *client_sock, int client_fd);

#endif