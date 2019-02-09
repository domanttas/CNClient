#include <stdio.h>
#include "Utils.h"

#define PORT "9034"
#define MAXDATASIZE 100

int main() {
    //Descriptor
    int sockfd;

    //Number of bytes
    int nBytes;
    int sBytes;

    //Address info in struct
    struct addrinfo hints, *serverinfo, *p;

    //Receiver
    int rv;

    //Data
    char buffer[MAXDATASIZE];
    char sendBuffer[MAXDATASIZE];

    char s[INET6_ADDRSTRLEN];

    int messageLength;

    //Descriptors
    fd_set read_fds;

    //Cleaning
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", PORT, &hints, &serverinfo)) != 0) {
        printf("Get address info: %s\n", gai_strerror(rv));
        exit(1);
    }

    //Looping through results and connecting to server
    for (p = serverinfo; p != NULL; p = p -> ai_next) {
        if ((sockfd = socket(p -> ai_family, p -> ai_socktype, p -> ai_protocol)) == -1) {
            printf("Client error while connecting");
            continue;
        }

        if (connect(sockfd, p -> ai_addr, p -> ai_addrlen) == -1) {
            close(sockfd);
            printf("Client error while connecting");
            continue;
        }

        break;
    }

    //If connection failed
    if (p == NULL) {
        printf(stderr, "Client failed to connect to host\n");
        exit(1);
    }

    //Network to protocol
    inet_ntop(p -> ai_family, get_in_addr((struct sockaddr *) p -> ai_addr), s, sizeof s);

    printf("Client connecting to %s\n", s);

    //Done, freeing memory
    freeaddrinfo(serverinfo);
    memset(&buffer, 0, sizeof buffer);

    printf("Connected to %s\n", s);
    printf("Enter your message:\n");

    //Loop for keeping connection with the server
    for(;;) {
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        FD_SET(0, &read_fds);

        select(sockfd + 1, &read_fds, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &read_fds)) {
            memset(&buffer, 0, MAXDATASIZE);
            read(sockfd, buffer, MAXDATASIZE);
            printf("Your received: ");
            printf("%s\n", buffer);
            printf("Enter your message:\n");
        } else if (FD_ISSET(0, &read_fds)) {
            if ((messageLength = read(0, &sendBuffer, MAXDATASIZE - 1)) == -1) {
                printf("Error while reading input!\n");
            }

            if (sendBuffer[0] == 'x' && sendBuffer[1] == '/') {
                break;
            }


            if ((send(sockfd, sendBuffer, messageLength, 0)) == -1) {
                printf("Error while sending data!\n");
            }

            memset(&sendBuffer, 0, MAXDATASIZE);
        }
    }

    close(sockfd);

    return 0;
}