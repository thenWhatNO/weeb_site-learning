#ifndef FILEM_H
#define FILEM_H

#include "incldes_libs.h"

typedef struct {
    FILE *html_file;
    FILE *chat_file;
} Files_struct;

//     working with files
int open_files(Files_struct *myfiles);
int close_files(Files_struct *myfiles);
int update_chat_file(Files_struct *myfiles, char *data, size_t d_size);
int read_file(Files_struct *myfiles, char file, char *buffer, size_t buffer_size);

#endif