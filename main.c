#include <stdio.h>
#include "Utils.h"

#define PORT "9034"
#define MAXDATASIZE 100

int main() {
    //Descriptor
    int sockfd;

    //Number of bytes
    int nBytes;

    //Address info in struct
    struct addrinfo hints, *serverinfo, *p;

    //Receiver
    int rv;

    //Data
    char buffer[MAXDATASIZE];

    char s[INET6_ADDRSTRLEN];

    //Cleaning
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", PORT, &hints, &serverinfo)) != 0) {
        fprintf(stderr, "Get address info: %s\n", gai_strerror(rv));
        exit(1);
    }

    //Looping through results and connecting to server
    for (p = serverinfo; p != NULL; p = p -> ai_next) {
        if ((sockfd = socket(p -> ai_family, p -> ai_socktype, p -> ai_protocol)) == -1) {
            perror("Client error while connecting");
            continue;
        }

        if (connect(sockfd, p -> ai_addr, p -> ai_addrlen) == -1) {
            close(sockfd);
            perror("Client error while connecting");
            continue;
        }

        break;
    }

    //If connection failed
    if (p == NULL) {
        fprintf(stderr, "Client failed to connect to host\n");
        exit(1);
    }

    //Network to protocol
    inet_ntop(p -> ai_family, get_in_addr((struct sockaddr *) p -> ai_addr), s, sizeof s);

    printf("Client connecting to %s\n", s);

    //Done, freeing memory
    freeaddrinfo(serverinfo);

    //Receiving
    if ((nBytes = recv(sockfd, buffer, MAXDATASIZE - 1, 0)) == -1) {
        perror("Error while receiving");
        exit(1);
    }

    buffer[nBytes] = '\0';

    printf("Client: received %s\n", buffer);

    close(sockfd);

    return 0;
}