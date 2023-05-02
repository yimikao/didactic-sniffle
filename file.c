#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "wfile.h"


filedata *file_data(char *filename)
{
    
    char *buffer, *p;
    struct stat buf;
    int bytes_read, bytes_remaining, total_bytes = 0;
    FILE *file;

    // get file properties. size, mode etc
    if (stat(filename, &buf) == -1)
    {
        return NULL;
    }

    // make sure its a regular file
    if (!(buf.st_mode & S_IFREG))
    {
        return NULL;
    }

    file = fopen(filename, "rb");
    if (file == NULL)
    {
        return NULL;
    }
    
    // allocate that many bytes
    bytes_remaining = buf.st_size;
    p = buffer = malloc(bytes_remaining);
    if (buffer == NULL) 
    {
        return NULL;
    }

    // read entire file
    while(bytes_read = fread(p, 1, bytes_remaining, file), bytes_read != 0 && bytes_remaining > 0) {
        if (bytes_read == -1 )
        {
            free(buffer);
            return NULL;
        }

        bytes_remaining -= bytes_read;
        p += bytes_read;
        total_bytes += bytes_read;
    }

    filedata *fd = malloc(sizeof *fd);
    if (fd == NULL)
    {
        free(buffer);
        return NULL;
    }

    fd->data = buffer;
    fd->size = total_bytes;

    return fd;
}

void file_free(filedata *fd)
{
    free(fd->data);
    free(fd);
}