
// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAXLINE 1024

// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *init_request = const_cast<char *>("index_hotmail.html");
    struct sockaddr_in servaddr{};

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket with the server address
    sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address));
    client_address.sin_family = AF_INET;
    if(inet_pton(AF_INET, "127.0.0.1", &client_address.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    client_address.sin_port = htons(static_cast<uint16_t>(12345));
    if (bind(sockfd, (const struct sockaddr *) &(client_address),
             sizeof(client_address)) < 0) {
        perror("init bind failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n, len;

    sendto(sockfd, (const char *)init_request, strlen(init_request),
           MSG_CONFIRM, (const struct sockaddr *) &servaddr,
           sizeof(servaddr));
    printf("Request to server sent.\n");

//    n = static_cast<int>(recvfrom(sockfd, (char *)buffer, MAXLINE,
//                                  MSG_WAITALL, (struct sockaddr *) &servaddr,
//                                  reinterpret_cast<socklen_t *>(&len)));
//    // Print server address here
//    buffer[n] = '\0';
//    printf("Server sent this(ACK supposedly): %s\n", buffer);
//    memset(buffer, 0, sizeof(buffer));
//    n = static_cast<int>(recvfrom(sockfd, (char *)buffer, MAXLINE,
//                                  MSG_WAITALL, (struct sockaddr *) &servaddr,
//                                  reinterpret_cast<socklen_t *>(&len)));
//    printf("Server sent this(data from child supposedly): %s\n", buffer);
//    // print server address here
//    const char *ack = "This is an ack from the client";
//    sendto(sockfd, ack, strlen(ack),
//           MSG_CONFIRM, (const struct sockaddr *) &servaddr,
//           sizeof(servaddr));
//    printf("Data ack sent from client to server.\n");
    close(sockfd);
    return 0;
}
