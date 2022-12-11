#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

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

#include "../include/url.h"


FILE * openFile(char* file, char* mode);

int setPasMode(int socketfd);

int downloadFile(url_t url);


#endif
