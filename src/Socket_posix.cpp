#include "Socket.h"

#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <iostream>

#define DEFAULT_BUFLEN 512


Socket::Socket(const char *ip, const char *port) {
    struct addrinfo hints{};
    struct addrinfo *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;        
    hints.ai_socktype = SOCK_DGRAM;    
    hints.ai_protocol = IPPROTO_UDP;   

    
    int res = getaddrinfo(ip, port, &hints, &result);
    if (res != 0) {
        throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(res)));
    }

    
    sendSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sendSocket == -1) {
        freeaddrinfo(result);
        throw std::runtime_error("socket creation failed: " + std::string(strerror(errno)));
    }

    serverAddr = *result->ai_addr;
    serverAddrLen = result->ai_addrlen;

    freeaddrinfo(result);
}


void Socket::sendToServer(const char *message) {
    ssize_t bytesSent = sendto(sendSocket, message, strlen(message), 0, &serverAddr, serverAddrLen);
    if (bytesSent == -1) {
        throw std::runtime_error("sendto failed: " + std::string(strerror(errno)));
    }

    std::cout << "Bytes sent: " << bytesSent << "\n";
    std::cout << "Message sent: " << message << "\n";
}

void Socket::receiveFromServer(char *message) {
    char recvbuf[DEFAULT_BUFLEN];
    memset(recvbuf, 0, DEFAULT_BUFLEN);

    ssize_t bytesReceived = recvfrom(sendSocket, recvbuf, DEFAULT_BUFLEN, 0, nullptr, nullptr);
    if (bytesReceived == -1) {
        throw std::runtime_error("recvfrom failed: " + std::string(strerror(errno)));
    }

    recvbuf[bytesReceived] = '\0'; 
    strcpy(message, recvbuf);

    std::cout << "Bytes received: " << bytesReceived << "\n";
    std::cout << "Message received: " << message << "\n";
}


Socket::Socket(const char *port) {
    struct addrinfo hints{};
    struct addrinfo *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;          
    hints.ai_socktype = SOCK_DGRAM;     
    hints.ai_flags = AI_PASSIVE;        

    
    int res = getaddrinfo(nullptr, port, &hints, &result);
    if (res != 0) {
        throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(res)));
    }

    
    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == -1) {
        freeaddrinfo(result);
        throw std::runtime_error("socket creation failed: " + std::string(strerror(errno)));
    }

   
    if (bind(listenSocket, result->ai_addr, result->ai_addrlen) == -1) {
        freeaddrinfo(result);
        close(listenSocket);
        throw std::runtime_error("bind failed: " + std::string(strerror(errno)));
    }

    freeaddrinfo(result);
}


void Socket::listen(char *message) {
    char recvbuf[DEFAULT_BUFLEN];
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    while (true) {
        std::cout << "Server is waiting for data...\n";

        memset(&clientAddr, 0, clientAddrLen);
        memset(recvbuf, 0, DEFAULT_BUFLEN);

        ssize_t bytesReceived = recvfrom(listenSocket, recvbuf, DEFAULT_BUFLEN, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (bytesReceived == -1) {
            std::cerr << "recvfrom failed: " << strerror(errno) << "\n";
            continue;
        }

        recvbuf[bytesReceived] = '\0'; 
        std::cout << "Received message: " << recvbuf << "\n";

        
        const char *response = "Message received by server!";
        ssize_t bytesSent = sendto(listenSocket, response, strlen(response), 0, (struct sockaddr *)&clientAddr, clientAddrLen);
        if (bytesSent == -1) {
            std::cerr << "sendto failed: " << strerror(errno) << "\n";
        } else {
            std::cout << "Response sent to client.\n";
        }
    }
}


Socket::~Socket() {
    if (sendSocket != -1) {
        close(sendSocket);
    }
    if (listenSocket != -1) {
        close(listenSocket);
    }
}
