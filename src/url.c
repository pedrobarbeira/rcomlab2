#include "../include/url.h"

#define FTP_HEADER "ftp:"
#define HEADER_LENGTH 6


int needsUser(char *arguments){
    for(int i = 1; i < strlen(arguments); i++){
        if(arguments[i] == '@'){
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int parseURL(char *arg, url_t *url){

    bzero((url_t*) url, sizeof(url_t));
    
    char* ftp = strtok(arg, "/");
    
    if(strcmp(FTP_HEADER, ftp)){
        perror("url type not supported\n");
        exit(EXIT_FAILURE);
    }

    char* arguments = strtok(NULL, "/");
    char* url_path = strtok(NULL, "/");


    if (arguments == NULL || url_path == NULL){
        printf("Some field is invalid!\n");
        exit(EXIT_FAILURE);
    }

    char* loginfield;
    char* hostname;

    if (needsUser(arguments)){
        loginfield = strtok(arguments, "@");
        hostname = strtok(NULL, "\0");
        char* username = strtok(loginfield, ":");
        char* pass = strtok(NULL, ":");

        if(username == NULL){
            username = "anonymous";
        }
        if(pass == NULL){
            pass = "pass";
        }

        strncpy(url->username, username, strlen(username));
        strncpy(url->pass, pass, strlen(pass));
        
    }
    else{
        hostname = strtok(arguments, "@");
    }


    strncpy(url->hostname, hostname, strlen(hostname));
    strncpy(url->path, url_path, strlen(url_path));

    if (!strcmp(url->hostname, "") || !strcmp(url->path, "")){
        printf("The URL is invalid!\n");
        return -1;
    }
    
    printf("User: %s\n", url->username);
    printf("Pass: %s\n", url->pass);
    printf("Host: %s\n", url->hostname);
    printf("URL Path: %s\n", url->path);

    char* ipaddress = "";
    if (getIP(ipaddress) != 0) return -1;

    strncpy(url->ip, ipaddress, strlen(ipaddress));

    printf("IP Address: %s\n", url->ip);

    return 0;
}