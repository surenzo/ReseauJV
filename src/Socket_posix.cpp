#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Socket.h"
#include <cstdlib>

Socket::Socket(const char* ip, const char* port) {
    // Initialisation des sockets
    SendSocket = socket(AF_INET, SOCK_DGRAM, 0);  // Création du socket pour l'envoi
    ListenSocket = socket(AF_INET, SOCK_DGRAM, 0);  // Création du socket pour l'écoute

    if (SendSocket < 0) {
        perror("Error creating SendSocket");
        return;
    }

    if (ListenSocket < 0) {
        perror("Error creating ListenSocket");
        return;
    }

    // Configuration des adresses IP et des ports
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(port));  // Utilisation du port passé en argument
    serverAddr.sin_addr.s_addr = inet_addr(ip);  // Utilisation de l'adresse IP passée en argument

    // Préparation de la structure sockaddr pour l'envoi
    result = reinterpret_cast<struct sockaddr*>(&serverAddr);
    result_size = sizeof(serverAddr);
}

Socket::~Socket() {
    // Fermeture des sockets ouverts
    if (SendSocket >= 0) {
        close(SendSocket);
    }
    if (ListenSocket >= 0) {
        close(ListenSocket);
    }
}

void Socket::sendToServer(const char* message) {
    ssize_t iResult = sendto(SendSocket, message, strlen(message), 0, result, result_size);
    if (iResult < 0) {
        perror("Error sending data");
    }
}

void Socket::receiveFromServer(char* recvbuf) {
    ssize_t iResult = recvfrom(SendSocket, recvbuf, 1024, 0, reinterpret_cast<struct sockaddr*>(&sender_addr), &sender_addr_size);
    if (iResult < 0) {
        perror("Error receiving data");
    }
}

void Socket::listen(char* buffer) {
    char recvbuf[1024];  // Buffeur de réception
    ssize_t iResult;

    // Attente de la réception de données sur le socket
    iResult = recvfrom(ListenSocket, recvbuf, sizeof(recvbuf), 0, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
    if (iResult < 0) {
        perror("Error receiving data");
    } else {
        // Traitement des données reçues
        printf("Received data: %s\n", recvbuf);
    }
}

void Socket::setServerAddr(const char* ip, const char* port) {
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(port));
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    result = reinterpret_cast<struct sockaddr*>(&serverAddr);
    result_size = sizeof(serverAddr);
}

void Socket::setListenAddr(const char* port) {
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(atoi(port));
    clientAddr.sin_addr.s_addr = INADDR_ANY;  // Écoute sur toutes les interfaces
}

void Socket::startListening() {
    int bindResult = bind(ListenSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), sizeof(clientAddr));
    if (bindResult < 0) {
        perror("Error binding ListenSocket");
        return;
    }

    printf("Server is waiting for data on port %s...\n", DEFAULT_PORT);
    ::listen(ListenSocket, 10);  // Commence à écouter sur le socket
}
