#ifndef _URL_H_
#define _URL_H_

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

#include "getip.h"

#define BUFSIZE 1024

typedef struct{
    char ip[BUFSIZE];
    char hostname[BUFSIZE];
    char path[BUFSIZE];
    char username[BUFSIZE];
    char pass[BUFSIZE];
} url_t;



int parseURL(char* arg, url_t *urlparsed);

int needsUser(char *arguments);


#endif
