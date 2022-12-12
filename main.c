#include "include/url.h"
#include "include/download.h"

int main(int argc, char** argv){
    if (argc != 2){
        printf("Incorrect number of arguments");
        return 1;
    }
    
    url_t url;

    if (parseURL(argv[1], &url) != 0){
        printf("Error while parsing");
        return -1;
    }

    if(transferFile(&url)){
        perror("Error transfering file\n");
        exit(1);
    }

    return 0;
    
}