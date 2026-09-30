#include "files_m.h"

struct File_unit{
    FILE *ptr;
    char *name;
    char *der;
};

struct Files_struct{
    int cap;
    int len;
    File_unit *FU;
};


int open_files(Files_struct *myfiles){
    //

    return 0;
}

int close_files(Files_struct *myfiles){
    // rebiuld

    return 0;
}

int update_chat_file(Files_struct *myfiles, char *data, size_t d_size){
    if(d_size > 9999){
        perror("data size too big to save in the file\n");
        return -1;
    }

    fseek(myfiles->chat_file, -2, SEEK_END);
    if(fwrite(data, 1, d_size, myfiles->chat_file) != d_size){
        perror("could not write data into chat file\n");
        rewind(myfiles->chat_file);
        return -1;
    }
    rewind(myfiles->chat_file);
    return 0;
}

int read_file(Files_struct *myfiles, char file, char *buffer, size_t buffer_size){
    FILE *ptr = NULL;
    if (file == 'm'){
        ptr = myfiles->html_file;
    }
    if(file == 'c'){
        ptr = myfiles->chat_file;
    }
    
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

    return full_l;
}