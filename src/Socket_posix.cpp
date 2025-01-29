#include "Socket.h"
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5555" 

Socket::Socket(const char *ip, const char *port) {
    // Créer la socket UDP
    SendSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (SendSocket < 0) {
        throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
    }

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // Résoudre l'adresse IP et le port
    int res = getaddrinfo(ip, port, &hints, &result);
    if (res != 0) {
        close(SendSocket);
        throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(res)));
    }

    // Connecter le socket au serveur
    res = connect(SendSocket, result->ai_addr, result->ai_addrlen);
    if (res < 0) {
        freeaddrinfo(result);
        close(SendSocket);
        throw std::runtime_error("connect failed: " + std::string(strerror(errno)));
    }

    freeaddrinfo(result);
}

void Socket::sendToServer(const char* message) {
    // Envoyer un message au serveur
    ssize_t bytesSent = send(SendSocket, message, strlen(message), 0);
    if (bytesSent < 0) {
        throw std::runtime_error("send failed: " + std::string(strerror(errno)));
    }
    std::cout << "Bytes sent: " << bytesSent << std::endl;
    std::cout << "Sent message: " << message << std::endl;
}

void Socket::receiveFromServer(char* message) {
    char recvbuf[DEFAULT_BUFLEN];
    ssize_t bytesReceived = recv(SendSocket, recvbuf, DEFAULT_BUFLEN, 0);
    if (bytesReceived < 0) {
        throw std::runtime_error("recv failed: " + std::string(strerror(errno)));
    }

    recvbuf[bytesReceived] = '\0'; // Null-terminate the received data
    std::cout << "Bytes received: " << bytesReceived << std::endl;
    std::cout << "Message received: " << recvbuf << std::endl;
}

Socket::Socket(const char *port) {
    // Créer un socket UDP pour le serveur
    ListenSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (ListenSocket < 0) {
        throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Ecoute sur toutes les interfaces réseau
    serverAddr.sin_port = htons(atoi(port));

    // Lier la socket au port
    if (bind(ListenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        close(ListenSocket);
        throw std::runtime_error("Bind failed: " + std::string(strerror(errno)));
    }
}

void Socket::listen(char* message) {
    char recvbuf[DEFAULT_BUFLEN];
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    std::cout << "Server is waiting for data on port " << DEFAULT_PORT << "...\n";
    while (true) {
        memset(recvbuf, 0, DEFAULT_BUFLEN);

        ssize_t bytesReceived = recvfrom(ListenSocket, recvbuf, DEFAULT_BUFLEN, 0, 
                                         (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (bytesReceived < 0) {
            std::cerr << "recvfrom failed: " << strerror(errno) << std::endl;
            break;
        }

        std::cout << "Received " << bytesReceived << " bytes from client: " << recvbuf << std::endl;

        // Répondre au client
        const char *response = "Message received by server!";
        ssize_t bytesSent = sendto(ListenSocket, response, strlen(response), 0, 
                                   (struct sockaddr*)&clientAddr, clientAddrLen);
        if (bytesSent < 0) {
            std::cerr << "sendto failed: " << strerror(errno) << std::endl;
            break;
        }

        std::cout << "Response sent to client.\n";
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
