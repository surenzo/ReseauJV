#include "Socket.h"
#include <stdexcept>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <cstring>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5555"

WSADATA wsaData;
int iResult;

struct addrinfo *result = NULL, hints;

SOCKET SendSocket = INVALID_SOCKET;
SOCKET ListenSocket = INVALID_SOCKET;

Socket::Socket(const char *ip, const char *port) {
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(iResult));
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    iResult = getaddrinfo(ip, port, &hints, &result);
    if (iResult != 0) {
        WSACleanup();
        throw std::runtime_error("getaddrinfo failed with error: " + std::to_string(iResult));
    }

    SendSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (SendSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        throw std::runtime_error("socket failed with error: " + std::to_string(WSAGetLastError()));
    }
}

void Socket::sendToServer(const char* message) {
    iResult = sendto(SendSocket, message, (int)strlen(message), 0, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(SendSocket);
        freeaddrinfo(result);
        WSACleanup();
        throw std::runtime_error("sendto failed with error: " + std::to_string(WSAGetLastError()));
    }
    printf("Bytes Sent: %d\n", iResult);
    printf("Sent message: %s\n", message);
}

void Socket::receiveFromServer(char* message) {
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    struct sockaddr_storage sender_addr;
    int sender_addr_size = sizeof(sender_addr);
    iResult = recvfrom(SendSocket, recvbuf, recvbuflen, 0, (struct sockaddr *)&sender_addr, &sender_addr_size);
    if (iResult > 0) {
        recvbuf[iResult] = '\0';  // Null-terminate the received data
        printf("Bytes received: %d\n", iResult);
        printf("Message received: %s\n", recvbuf);
        strcpy(message, recvbuf);
    } else if (iResult == 0) {
        printf("Connection closed\n");
    } else {
        throw std::runtime_error("recvfrom failed with error: " + std::to_string(WSAGetLastError()));
    }
}

Socket::Socket(const char *port) {
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(iResult));
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        WSACleanup();
        throw std::runtime_error("getaddrinfo failed with error: " + std::to_string(iResult));
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        throw std::runtime_error("socket failed with error: " + std::to_string(WSAGetLastError()));
    }

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        throw std::runtime_error("Bind failed with error: " + std::to_string(WSAGetLastError()));
    }

    freeaddrinfo(result);
}

void Socket::listen(char* message) {
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    printf("Server is waiting for data on port %s...\n", DEFAULT_PORT);
    while (1) {
        printf("Waiting to receive data...\n");

        ZeroMemory(&clientAddr, clientAddrLen);
        ZeroMemory(recvbuf, DEFAULT_BUFLEN);

        iResult = recvfrom(ListenSocket, recvbuf, recvbuflen, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (iResult == SOCKET_ERROR) {
            printf("recvfrom failed with error: %d\n", WSAGetLastError());
            break;
        }

        printf("Received %d bytes from client: %s\n", iResult, recvbuf);

        const char *response = "Message received by server!";
        iResult = sendto(ListenSocket, response, (int)strlen(response), 0, (struct sockaddr*)&clientAddr, clientAddrLen);
        if (iResult == SOCKET_ERROR) {
            printf("sendto failed with error: %d\n", WSAGetLastError());
            break;
        }

        printf("Response sent to client.\n");
    }
}

Socket::~Socket() {
    if (SendSocket != INVALID_SOCKET) {
        closesocket(SendSocket);
    }
    if (ListenSocket != INVALID_SOCKET) {
        closesocket(ListenSocket);
    }
    freeaddrinfo(result);
    WSACleanup();
}
