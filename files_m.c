#include "files_m.h"

struct File_unit{
    char *name;
    char *der;
};

struct Files_struct{
    int cap;
    int len;
    File_unit *FU;
};

Files_struct *init_File_struct(){
    Files_struct *Fu = malloc(sizeof(Files_struct));
    Fu->cap = 5;
    Fu->len = 0;
    Fu->FU = malloc(sizeof(File_unit) * 5);
    return Fu;
}

int add_file(Files_struct *myfiles, char *name, char *der){
    myfiles->FU->der = malloc(strlen(der));
    myfiles->FU->name = malloc(strlen(name));

    strcpy(myfiles->FU->name, name);
    strcpy(myfiles->FU->der, der);

    myfiles->len++;

    return 0;
}

int free_file(Files_struct *myfiles){
    for (int i = 0; i < myfiles->len; i++){
        free(myfiles->FU[i].der);
        myfiles->FU[i].der = NULL;
        free(myfiles->FU[i].name);
        myfiles->FU[i].name = NULL;
    }

    free(myfiles->FU);
    myfiles->FU = NULL;

    free(myfiles);
    myfiles = NULL;

    return 0;
}

int fine_fd_by_name(Files_struct *myfiles, char *name){
    for (int i = 0; i < myfiles->len; i++){
        if(strcmp(myfiles->FU[i].name, name) == 0) return i;
    }
    return -1;
}

int update_file(    Files_struct *myfiles, 
                    char *name, 
                    char *data, 
                    size_t d_size, 
                    int offset
                )
    {
    int id = fine_fd_by_name(myfiles, name);

    FILE *file = fopen(myfiles->FU[id].der, "a");

    if (file == NULL){
        perror("could not open file\n");
        return -1;
    }

    fseek(file, -offset, SEEK_END);
    if(fwrite(data, 1, d_size, file) != d_size){
        perror("could not write data into chat file\n");
        rewind(file);
        return -1;
    }
    rewind(file);

    fclose(file);
    return 0;
}

int read_file(Files_struct *myfiles, char *name, char *buffer, size_t buffer_size){
    int id = fine_fd_by_name(myfiles, name);
    FILE *ptr = fopen(myfiles->FU[id].der, "r");;

    if(ptr == NULL){
        return -1;
    }

    fseek(ptr, 0, SEEK_END);
    long file_size = ftell(ptr);
    rewind(ptr);

    if(buffer_size < file_size+1){
        perror("the buffer too small");
        return -1;
    }

    size_t full_l = fread(buffer, 1, file_size, ptr);
    (buffer)[full_l] = '\0';

    fclose(ptr);

    return full_l;
}