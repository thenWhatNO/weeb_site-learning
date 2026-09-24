#include "html_data.h"
#include "datagram.h"
#include "files_m.h"

typedef struct{
    char *html_msg;
    int len;
    int cap;
    int updated;
} HTML_datagram; 


int applay_haders(  char *buff, 
                    size_t buff_size, 
                    char *hadder, 
                    char *data, 
                    size_t data_l
                )
    {
    char *HADER_C = strcasestr(hadder, "Content-Length");
    if(HADER_C != NULL){
        snprintf(buff, buff_size, hadder, data_l, data);
    } else {
        snprintf(buff, buff_size, hadder, data);
    }
    return 0;
}




HTML_datagram *init_html_struct(){

    HTML_datagram *html_str = malloc(sizeof(HTML_datagram));

    html_str->cap = 10;
    html_str->len = 0;
    html_str->html_msg = calloc(html_str->cap, sizeof(char));

    if(html_str->html_msg == NULL){return -1;}

    return html_str;
}

int write_msg_to_html(HTML_datagram *html_data, char *msg, size_t msg_len){
    if(html_data->cap < msg_len){
        html_data->cap = msg_len;
        html_data->html_msg = realloc(html_data->html_msg, html_data->cap);
    }

    html_data->len = msg_len;

    if (html_data->html_msg == NULL){return -1;}
    memset(html_data->html_msg, 0, html_data->cap);
    strcpy(html_data->html_msg, msg);

    return 0;
}

int free_html_stract(HTML_datagram *html_data){
    free(html_data->html_msg);
    html_data->html_msg = NULL;

    return 0;
}

int generate_data(  HTML_datagram *html_data, 
                    Files_struct *myfiles, 
                    Datagram_store *DTgrams,
                    char *hadr
                )
    {
    
    char html_tamply[8000];
    size_t html_len;

    html_len = read_file(myfiles, 'm', html_tamply, 8000);

    size_t total_len = html_len + strlen(hadr);
    char data_buff[total_len];
    
    snprintf(data_buff, total_len, hadr, html_len, html_tamply);

    write_msg_to_html(html_data, data_buff, total_len);

    return total_len;
}

int generate_msg(Files_struct *myfiles, char *buff){
    char chat_data[8000];
    size_t chat_len;

    chat_len = read_file(myfiles, 'c', chat_data, 8000);
    size_t total_size = chat_len + strlen(MAEG_HADR);

    snprintf(buff, total_size, MAEG_HADR, chat_len, chat_data);

    return total_size;
}

