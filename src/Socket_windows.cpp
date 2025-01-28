#include "Socket.h"
#include <stdexcept>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512

class SocketWindows : public Socket {
public:
    SocketWindows(const char *ip, const char *port);
    SocketWindows(const char *port);
    ~SocketWindows();

    void sendToServer(const std::string &message) override;
    void receiveFromServer(std::string &message) override;
    void listen(std::string &message) override;

private:
    SOCKET socketFD = INVALID_SOCKET;
    sockaddr_in serverAddr{};
};

// Initialisation Winsock
SocketWindows::SocketWindows(const char *ip, const char *port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    socketFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketFD == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("socket creation failed");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(std::stoi(port));
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);
}

// Initialisation serveur
SocketWindows::SocketWindows(const char *port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    socketFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketFD == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("socket creation failed");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(std::stoi(port));

    if (bind(socketFD, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(socketFD);
        WSACleanup();
        throw std::runtime_error("bind failed");
    }
}

// Destructor
SocketWindows::~SocketWindows() {
    closesocket(socketFD);
    WSACleanup();
}

// Méthodes spécifiques
void SocketWindows::sendToServer(const std::string &message) {
    int bytesSent = sendto(socketFD, message.c_str(), message.size(), 0, (sockaddr *)&serverAddr, sizeof(serverAddr));
    if (bytesSent == SOCKET_ERROR) {
        throw std::runtime_error("sendto failed");
    }
}

void SocketWindows::receiveFromServer(std::string &message) {
    char buffer[DEFAULT_BUFLEN] = {0};
    int serverAddrLen = sizeof(serverAddr);

    int bytesReceived = recvfrom(socketFD, buffer, DEFAULT_BUFLEN, 0, (sockaddr *)&serverAddr, &serverAddrLen);
    if (bytesReceived == SOCKET_ERROR) {
        throw std::runtime_error("recvfrom failed");
    }

    buffer[bytesReceived] = '\0';
    message = std::string(buffer);
}

void SocketWindows::listen(std::string &message) {
    char buffer[DEFAULT_BUFLEN] = {0};
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    int bytesReceived = recvfrom(socketFD, buffer, DEFAULT_BUFLEN, 0, (sockaddr *)&clientAddr, &clientAddrLen);
    if (bytesReceived == SOCKET_ERROR) {
        throw std::runtime_error("recvfrom failed");
    }

    buffer[bytesReceived] = '\0';
    message = std::string(buffer);

    const char *response = "Acknowledged";
    sendto(socketFD, response, strlen(response), 0, (sockaddr *)&clientAddr, clientAddrLen);
}
