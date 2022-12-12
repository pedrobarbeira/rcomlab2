#include "download.h"

/* FTP Replys */
// 1yz Positive Preliminary reply
// 2yz Positive Completion reply
// 3yz Positive Intermediate reply
// 4yz Transient Negative Completion reply
// 5yz Permanent Negative Completion reply

int socketCreateConnect( char *ipAddress, int port){
    int tempSocket;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ipAddress);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((tempSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }

    /*connect to the server*/
    if (connect(tempSocket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }
    return tempSocket;
}

int writeToSocket(int sockfd, char *command, char *arguments){
    int commandSize = strlen(command);
    int argumentsSize = strlen(arguments);

    if (write(sockfd, command, commandSize) != commandSize) {
        perror("writing command to socket");
        exit(-1);
    }
    if(arguments != NULL){
        if (write(sockfd, arguments, argumentsSize) != argumentsSize) {
            perror("writing arguments to socket");
            exit(-1);
        }
    }
    write(sockfd, "\n", 1);
    return 0;
}

int readFromSocket(int sockfd){
    char code;
	char *response = malloc(BUFSIZE);
	
	size_t n = 0;
	ssize_t read;

	FILE* fp = fdopen(sockfd, "r");
	while((read = getline(&response, &n, fp)) != -1) {
		if(response[3] == ' ') break;
	}
    printf("Reply: %s", response);
	response[BUFSIZE - 1] = '\0';
	code = response[0];
	free(response);
	return code;
}

int login(int sockfd, char *user, char* password){
    writeToSocket(sockfd, "USER ", user);
    char socketReply;
	socketReply = readFromSocket(sockfd);
	if(socketReply == '4' || socketReply == '5'){
        perror("writing user to socket");
		close(sockfd);
		exit(-1);
	}

	writeToSocket(sockfd, "PASS ", password);
	socketReply = readFromSocket(sockfd);
	if(socketReply == '4' || socketReply == '5'){
        perror("writing password to socket");
		close(sockfd);
		exit(-1);
	}
    return 0;
}

int setPassiveMode(int sockfd, response_pasv_t *res){

	writeToSocket(sockfd, "PASV ", "");

    char *response = malloc(BUFSIZE);
	read(sockfd, response, BUFSIZE);
	printf("Reply: %s", response);

	if(response[0] == '3' || response[0] == '4' || response[0] == '5') {
        perror("setting passive mode \n");
		exit(-1);
    }
    
    //parse PASV
    strtok(response, "(");
    char * url = strtok(NULL, ")");

	int ip[4], port[2];
	sscanf(url, "%d,%d,%d,%d,%d,%d", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]);

	res->port = port[0] * 256 + port[1];
	sprintf(res->ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

	free(response);
    return 0;
}

char * getFilename(char * path) {
    char * filename = path, *aux;
    for (aux = path; *aux; aux++) {
        if (*aux == '/' || *aux == '\\' || *aux == ':') {
            filename = aux + 1;
        }
    }
    return filename;
}

int downloadFile(int fd, char *fileName){
	FILE *fp = fopen(fileName, "w");
	
	if(fp == NULL){
		perror("Error opening file");
		exit(1);
	}

	char buffer[BUFSIZE];
	int bytes_read;
	while((bytes_read = read(fd, buffer, BUFSIZE)) > 0){
		fwrite(buffer, bytes_read, 1, fp);
	}

	fclose(fp);
    
    return 0;
}

int disconnectSocket(int sockfd){
    writeToSocket(sockfd, "QUIT ", "");
    char socketReply;
    socketReply = readFromSocket(sockfd);
	if(socketReply == '4' || socketReply == '5'){
        perror("disconnecting socket \n");
		close(sockfd); 
		exit(-1);
	}
    return 0;
}

int closeSocket(int sockfd){
    if (close(sockfd)<0) {
        perror("closing socket");
        exit(-1);
    }
    return 0;
}


int transferFile(url_t *url){
    int sockfd;
    //Connect to socket
    if ((sockfd = socketCreateConnect(url->ip,PORT)) < 0){
        perror("socketCreateConnect()");
        exit(-1);
    }
    //Read initial response
    char socketReply;
    socketReply = readFromSocket(sockfd);
    if(socketReply == '4' || socketReply == '5'){
		if(closeSocket(sockfd) != 0) {
            exit(-1);
        }
	}

    //Login to serve
    if(login(sockfd, url->username, url->pass) != 0){
        perror("login()");
        exit(-1);
    }
     
    //Enter passive mode
    response_pasv_t res;
    if(setPassiveMode(sockfd,&res) != 0){
        perror("setPassiveMode()");
        exit(-1);
    }
    //Initiating data socket
    int datafd;
    if ((datafd = socketCreateConnect(url->ip, res.port)) < 0){
        perror("Error initializing data connection!\n");
        exit(-1);
    }

    //Resquesting file 
    char *fileName = getFilename(url->path);
    writeToSocket(sockfd, "RETR ", url->path);
	socketReply = readFromSocket(sockfd);
	if(socketReply == '4' || socketReply == '5'){
        perror("requesting file \n");
		close(sockfd); 
        close(datafd);
		exit(-1);
	}
    //Downloading file
    if(downloadFile(datafd, fileName) != 0){
        perror("downloadFile()");
        exit(-1);
    }

    //Reading retrival file response
    socketReply = readFromSocket(sockfd);
    if(socketReply == '4' || socketReply == '5'){
		if(closeSocket(sockfd) != 0) exit(-1);
	}
    
    //Close connection
    if(disconnectSocket(sockfd) != 0){
        perror("disconnectSocket()");
    }
    //Close socket
    if(closeSocket(sockfd) != 0){
        perror("closeSocket()");
    }
    if(closeSocket(datafd) != 0){
        perror("closeSocket()");
    }
    return 0;
}