#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */

void error(const char *msg) { perror(msg); exit(1); }

int confirm(const char *arg)
{
    /* first what are we going to send and where are we going to send it? */
    int portno =        80;
    char *host =        "voio.io";
    char *message_fmt = "GET /ready?user=%s HTTP/1.0\n";

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, total;
    char message[1024];

    /* fill in the parameters */
    sprintf(message, message_fmt, arg);
    printf("Request: %s\n",message);

    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    /* lookup the ip address */
    server = gethostbyname(host);
    if (server == NULL) error("ERROR, no such host");

    /* fill in the structure */
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    /* connect the socket */
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    /* send the request */
    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd,message+sent,total-sent);
        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);

    /* close the socket */
    close(sockfd);

    return 0;
}

#include "SimpleFaceStrategy.h"
#include "gifs/gifcv.h"

int main(int argc, char** argv) {
	if(argc != 3) {
		std::cout << "Usage: main <filename> <pathprefix>" << std::endl;
		return 1;
	}
	try {
		std::vector<std::string> sp = split(std::string(argv[2]), '/');
		Filter f;
		SimpleFaceStrategy ls;

		std::cout << "Starting analysis" << std::endl;
		std::vector<Timestamp> timestamps = ls.processVideo(std::string(argv[1]), 3); 
		std::cout << "Starting gif production with : " << timestamps.size() << std::endl;
		f.extractGifs(std::string(argv[1]), std::string(argv[2]), 0, timestamps);
		//confirm(sp[4].c_str());
	} catch(const char* s) {
		std::cout << std::string(s) << std::endl;
	}
	return 0;
}
