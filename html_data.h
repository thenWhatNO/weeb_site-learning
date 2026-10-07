#ifndef HTMLDATA_H
#define HTMLDATA_H

#define MAIN_HADR "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: %zu\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n%s"
#define ALIVE_HADR "HTTP/1.1 200 OK\r\nContent-Type: text/event-stream\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n"
#define MAEG_HADR "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\nContent-Length: %zu\r\n\r\n%s"

typedef struct HTML_page HTML_page;
typedef struct HTML_page_store HTML_page_store;

HTML_page *get_html_page(HTML_page_store *HPS, int idx);

char *get_HP_name(HTML_page *hp);
char *get_HP_link(HTML_page *hp);
char *get_HP_data(HTML_page *hp);
int get_HP_size(HTML_page *hp);

HTML_page_store *init_html_struct();
int add_html_page(HTML_page_store *hps, char *name, int name_size, char *link, int link_size, char *file_link);

// return the index of the page that the user want to use
int get_html_gape_by_link(HTML_page_store *hps, char *link, int link_size);
int free_html_page_store(HTML_page_store *hps);

int applay_haders(char *buff, int buff_size, char *hadder, char *data , int data_l);

#endif