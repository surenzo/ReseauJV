#include "Socket.h"
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <iostream>


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5555"

Socket::Socket(const char *ip, const char *port) {
    struct addrinfo hints, *result;
    int iResult;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    iResult = getaddrinfo(ip, port, &hints, &result);
    if (iResult != 0) {
        throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(iResult)));
    }

    SendSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (SendSocket == -1) {
        freeaddrinfo(result);
        throw std::runtime_error("socket failed: " + std::string(strerror(errno)));
    }
}

void Socket::sendToServer(const char* message) {
    struct addrinfo *result;
    int iResult = sendto(SendSocket, message, strlen(message), 0, result->ai_addr, result->ai_addrlen);
    if (iResult == -1) {
        throw std::runtime_error("sendto failed: " + std::string(strerror(errno)));
    }
}

void Socket::receiveFromServer(char* message) {
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    struct sockaddr_storage sender_addr;
    socklen_t sender_addr_size = sizeof(sender_addr);
    int iResult = recvfrom(SendSocket, recvbuf, recvbuflen, 0, (struct sockaddr *)&sender_addr, &sender_addr_size);
    if (iResult > 0) {
        recvbuf[iResult] = '\0';
        std::strcpy(message, recvbuf);
    } else if (iResult == 0) {
        std::cout << "Connection closed\n";
    } else {
        throw std::runtime_error("recvfrom failed: " + std::string(strerror(errno)));
    }
}

Socket::Socket(const char *port) {
    struct addrinfo hints, *result;
    int iResult;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, port, &hints, &result);
    if (iResult != 0) {
        throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(iResult)));
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == -1) {
        freeaddrinfo(result);
        throw std::runtime_error("socket failed: " + std::string(strerror(errno)));
    }

    iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
    if (iResult == -1) {
        freeaddrinfo(result);
        throw std::runtime_error("bind failed: " + std::string(strerror(errno)));
    }

    freeaddrinfo(result);
}

void Socket::listen(char* message) {
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    while (1) {
        memset(recvbuf, 0, sizeof(recvbuf));

        int iResult = recvfrom(ListenSocket, recvbuf, recvbuflen, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (iResult == -1) {
            throw std::runtime_error("recvfrom failed: " + std::string(strerror(errno)));
        }

        std::cout << "Received message: " << recvbuf << "\n";

        const char* response = "Message received by server!";
        sendto(ListenSocket, response, strlen(response), 0, (struct sockaddr *)&clientAddr, clientAddrLen);
    }
}

Socket::~Socket() {
    if (SendSocket != -1) {
        close(SendSocket);
    }
    if (ListenSocket != -1) {
        close(ListenSocket);
    }
}
