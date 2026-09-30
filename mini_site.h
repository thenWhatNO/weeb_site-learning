#ifndef MINI_SITE_H
#define MINI_SITE_H

#include "client.h"
#include "server.h"
#include "datagram.h"
#include "files_m.h"
#include "html_data.h"
#include "mini_site.h"
#include "incldes_libs.h"

int listin_server(Datagram_store *DTgrams, Server_socket *server_fd ,Clients_socket *client_socket , size_t timeout);

#endif