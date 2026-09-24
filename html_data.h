#ifndef HTMLDATA_H
#define HTMLDATA_H

#define MAIN_HADR "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: %zu\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n%s"
#define ALIVE_HADR "HTTP/1.1 200 OK\r\nContent-Type: text/event-stream\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n"
#define MAEG_HADR "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s"

#include "incldes_libs.h"

typedef struct HTML_datagram HTML_datagram;

HTML_datagram *init_html_struct();
int applay_haders(char *buff, size_t buff_size, char *hadder, char *data , size_t data_l);

int write_msg_to_html(HTML_datagram *html_data, char *msg, size_t msg_len);
int free_html_stract(HTML_datagram *html_data);

int generate_data(HTML_datagram *html_data, Files_struct *myfiles, Datagram_store *DTgrams, char *hadr);
int generate_msg(Files_struct *myfiles, char *buff);

#endif