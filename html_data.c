#include "html_data.h"
#include "incldes_libs.h"

struct HTML_page{
    char *name;
    char *link;
    char *data;
    int size;
};

struct HTML_page_store{
    int len;
    int cap;
    HTML_page *pages;
};

HTML_page *get_html_page(HTML_page_store *hps, int idx){return &hps->pages[idx];};

char *get_HP_name(HTML_page *hp){return hp->name;}
char *get_HP_link(HTML_page *hp){return hp->link;}
char *get_HP_data(HTML_page *hp){return hp->data;}
int get_HP_size(HTML_page *hp){return hp->size;}


HTML_page_store *init_html_struct(){
    HTML_page_store *hps = malloc(sizeof(HTML_page_store));

    hps->cap = 10;
    hps->len = 0;
    hps->pages = malloc(sizeof(HTML_page) * hps->cap);

    return hps;
}

int add_html_page(  HTML_page_store *hps, 
                    char *name,
                    int name_size,
                    char *link,
                    int link_size,
                    char *file_link
                )
    {

    if(hps->len >= hps->cap){
        hps->cap += 10;
        hps->pages = realloc(hps->pages, hps->cap);
    }

    FILE *fl = fopen(file_link, "r");
    fseek(fl, 0, SEEK_END);
    long file_l = ftell(fl);
    rewind(fl);
    
    hps->pages[hps->len].data = malloc(file_l+1);
    hps->pages[hps->len].size = file_l+1;

    size_t full_l = fread(hps->pages[hps->len].data, 1, file_l, fl);
    (hps->pages[hps->len].data)[full_l] = '\0';

    hps->pages[hps->len].name = malloc(name_size);
    hps->pages[hps->len].link = malloc(link_size);

    strcpy(hps->pages[hps->len].name, name);
    strcpy(hps->pages[hps->len].link, link);

    hps->len++;

    fclose(fl);
    fl = NULL;

    return 0;
}

int get_html_gape_by_link(HTML_page_store *hps, char *link, int link_size){
    for (int i = 0; i < hps->len; i++){
        if(strncmp(hps->pages[i].link, link, link_size) == 0){
            return i;
        }
    }
    return -1;
}

int free_html_page_store(HTML_page_store *hps){
    for (int i = 0; i < hps->len; i++){
        free(hps->pages[i].name);
        free(hps->pages[i].link );
        free(hps->pages[i].data );
        hps->pages[i].name = NULL;
        hps->pages[i].link = NULL;
        hps->pages[i].data = NULL;
    }

    free(hps);
    hps = NULL;

    return 0;
}

int applay_haders(  char *buff, 
                    int buff_size, 
                    char *hadder, 
                    char *data, 
                    int data_l
                )
    {
    char *HADER_C = strcasestr(hadder, "Content-Length");
    if(HADER_C != NULL){
        snprintf(buff, buff_size, hadder, data_l, data);
    } else {
        snprintf(buff, buff_size, hadder, data);
    }
    return strlen(buff);
}