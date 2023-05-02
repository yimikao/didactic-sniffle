#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/file.h>
#include <fcntl.h>
#include "net.h"
#include "file.h"

#define PORT "8080"

#define SERVER_FILES "./files"

void error_resp(int fd)
{
    filedata *filed;
    char *path = calloc(30, sizeof(char));
    sprintf(path, "%s/%s",SERVER_FILES, "404.html");
    filed = file_data(path);
    resp_send(fd, filed->data, filed->size, "HTTP/1.1 404 NOT FOUND", "text/html");
    free(path);
}

void page_resp(int fd, char *path)
{
    filedata *filed;
    char *flpath = calloc(30, sizeof(char));
    sprintf(flpath, "%s%s", SERVER_FILES, path);
    filed = file_data(flpath);
    resp_send(fd, filed->data, filed->size, "HTTP/1.1 200 OK", "text/html");
    file_free(filed);
    free(flpath);
}


void simple_resp(int fd)
{
    char *response_body = calloc(12, sizeof(char));
    
    int bytes_wt = sprintf(
    response_body,
    "%s"
    , "Hello World");

    resp_send(
    fd,
    (char*)response_body,
    bytes_wt,
    "HTTP/1.1 200 OK",
    "text/plain");
    free(response_body);
}


void handle_conn(int fd) {
    // buffer to read request into
    char buffer[65555];
    memset(buffer, 0, sizeof buffer);
    recv(fd, buffer, sizeof buffer, 0);
    // get header to determine action
    // GET /path HTTP/1.1\r\n
    char header[25];
    memset(header, 0, sizeof header);
    char *newline_pos;
    // string char first occurs
    newline_pos = strchr(buffer, '\n');
    if (newline_pos != NULL) {
        newline_pos--;
        int len = newline_pos - buffer;
        strncpy(header, buffer, len);
    }
    char *at_path;
    at_path = strstr(header, "/");
    if (strncmp(at_path, "/ ", 2) == 0)
    {
        simple_resp(fd);
        return;
    }
    // send file 
    char *path_end = strstr(header, " HTTP");
    if (path_end == NULL) {
        perror("path_len");
        return;
    }
    path_end--;
    int path_len = path_end - at_path;
    path_len = path_len + 1;
    char *path = calloc(20, sizeof(char));
    // copy /whatever
    strncpy(path, at_path, path_len);

    page_resp(fd, path);
    free(path);
}



int main() 
{

    struct sockaddr_storage cl_info;
    int sockfd = get_listener_or_die(PORT);
    if (sockfd < 0)
    {
        perror("get socket");
        exit(1);
    }

    printf("webserver: waiting for connections on port %s...\n", PORT);

    while(1) {
        socklen_t sin_size = sizeof cl_info;
        int fd = accept(sockfd, (struct sockaddr*)&cl_info, &sin_size);
        if (fd < 0) {
            perror("accept");
            continue;
        }

        handle_conn(fd);
        if (shutdown(fd, SHUT_RDWR) == -1)
        {
            perror("shutdown failed");
        }
        close(fd);
    }
    close(sockfd);
    return 0;

}