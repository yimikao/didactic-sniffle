#ifndef _FILELS_H_
#define _FILELS_H_

typedef struct filedata {
    int size;
    void *data;
} filedata;

filedata *file_data(char *filename);
void file_free(filedata *fd);

#endif