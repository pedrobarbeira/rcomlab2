#include "include/url.h"

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

    //if (download(&url) != 0) return -1;
    return 0;
    
}