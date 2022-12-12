#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "url.h"

typedef struct{
    char ip[1024];
    int port;
} response_pasv_t;

#define PORT 21

/* FTP Replys */
// 1yz Positive Preliminary reply
// 2yz Positive Completion reply
// 3yz Positive Intermediate reply
// 4yz Transient Negative Completion reply
// 5yz Permanent Negative Completion reply

FILE* openFile(char* fileName, char* mode);

int socketCreateConnect( char *ipAddress, int port);

int writeToSocket(int sockfd, char *command, char *arguments);

int readFromSocket(int sockfd);

int login(int sockfd, char *user, char* password);

int setPassiveMode(int sockfd, response_pasv_t *res);

char* getFilename(char* urlPath);

int downloadFile(int fd, char* fileName);

int disconnectSocket(int sockfd);

int closeSocket(int sockfd);

int transferFile(url_t *url);

#endif
