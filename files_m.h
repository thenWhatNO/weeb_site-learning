#ifndef FILEM_H
#define FILEM_H

#include "incldes_libs.h"

typedef struct Files_struct Files_struct;
typedef struct File_unit File_unit;

Files_struct *init_File_struct();
int add_file(Files_struct *myfiles, char *name, char *der);
int fine_fd_by_name(Files_struct *myfiles, char *name);
int update_file(Files_struct *myfiles, char *name, char *data, size_t d_size, int offset);
int read_file(Files_struct *myfiles, char file, char *buffer, size_t buffer_size);

#endif