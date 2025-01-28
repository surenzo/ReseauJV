#include "Socket.h"
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEFAULT_BUFLEN 512


Socket::Socket(const char *ip, const char *port) {
    // Créer un socket UDP
    SendSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (SendSocket < 0) {
        throw std::runtime_error("Socket creation failed with error: " + std::to_string(errno));
    }

    // Résoudre l'adresse du serveur
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(port));
    if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid IP address: " + std::string(ip));
    }

    result = reinterpret_cast<struct sockaddr*>(&serverAddr);
    result_size = sizeof(serverAddr);
}

void Socket::sendToServer(const char* message) {
    // Envoyer un message au serveur
    ssize_t iResult = sendto(SendSocket, message, strlen(message), 0, result, result_size);
    if (iResult < 0) {
        throw std::runtime_error("sendto failed with error: " + std::to_string(errno));
    }

    printf("Bytes Sent: %ld\n", iResult);
    printf("Sent message: %s\n", message);
}

void Socket::receiveFromServer(char* message) {
    char recvbuf[DEFAULT_BUFLEN];
    socklen_t recvbuflen = DEFAULT_BUFLEN;

    struct sockaddr_storage sender_addr;
    socklen_t sender_addr_size = sizeof(sender_addr);

    ssize_t iResult = recvfrom(SendSocket, recvbuf, recvbuflen, 0, reinterpret_cast<struct sockaddr*>(&sender_addr), &sender_addr_size);
    if (iResult > 0) {
        recvbuf[iResult] = '\0'; 
        printf("Bytes received: %ld\n", iResult);
        printf("Message received: %s\n", recvbuf);
    } else if (iResult == 0) {
        printf("Connection closed\n");
    } else {
        throw std::runtime_error("recvfrom failed with error: " + std::to_string(errno));
    }
}

Socket::Socket(const char* port) {
    
    ListenSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (ListenSocket < 0) {
        throw std::runtime_error("Socket creation failed with error: " + std::to_string(errno));
    }

    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(port));
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    
    int iResult = bind(ListenSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr));
    if (iResult < 0) {
        throw std::runtime_error("Bind failed with error: " + std::to_string(errno));
    }
}

void Socket::listen(char* message) {
    char recvbuf[DEFAULT_BUFLEN];
    socklen_t recvbuflen = DEFAULT_BUFLEN;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    
    printf("Server is waiting for data on port %s...\n", DEFAULT_PORT);
    while (true) {
        printf("Waiting to receive data...\n");

       
        memset(&clientAddr, 0, clientAddrLen);
        memset(recvbuf, 0, DEFAULT_BUFLEN);

        
        ssize_t iResult = recvfrom(ListenSocket, recvbuf, recvbuflen, 0, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
        if (iResult < 0) {
            printf("recvfrom failed with error: %d\n", errno);
            break;
        }

        
        printf("Received %ld bytes from client: %s\n", iResult, recvbuf);

        
        const char *response = "Message received by server!";
        iResult = sendto(ListenSocket, response, strlen(response), 0, reinterpret_cast<struct sockaddr*>(&clientAddr), clientAddrLen);
        if (iResult < 0) {
            printf("sendto failed with error: %d\n", errno);
            break;
        }

        printf("Response sent to client.\n");
    }
}

Socket::~Socket() {
    if (SendSocket >= 0) {
        close(SendSocket);
    }
    if (ListenSocket >= 0) {
        close(ListenSocket);
    }
}
