#ifndef HTMLDATA_H
#define HTMLDATA_H

#define MAIN_HADR "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: %zu\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n%s"
#define ALIVE_HADR "HTTP/1.1 200 OK\r\nContent-Type: text/event-stream\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n"
#define MAEG_HADR "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s"

typedef struct HTML_page HTML_page;
typedef struct HTML_page_store HTML_page_store;

HTML_page_store *init_html_struct();
int add_html_page(HTML_page_store *hps, char *name, int name_size, char *link, int link_size, char *file_link);
int free_html_page_store(HTML_page_store *hps);

int applay_haders(char *buff, size_t buff_size, char *hadder, char *data , size_t data_l);

#endif