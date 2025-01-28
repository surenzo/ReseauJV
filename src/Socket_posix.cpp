#include "Socket.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DEFAULT_BUFLEN 512

class SocketPosix : public Socket {
public:
    // Constructeur pour le client
    SocketPosix(const char *ip, const char *port);

    // Constructeur pour le serveur
    SocketPosix(const char *port);

    // Destructeur
    ~SocketPosix();

    // Méthodes spécifiques à POSIX
    void sendToServer(const std::string &message) override;
    void receiveFromServer(std::string &message) override;
    void listen(std::string &message) override;

private:
    int socketFD = -1;
    sockaddr_in serverAddr{};
};

// ** Constructeur pour le client **
SocketPosix::SocketPosix(const char *ip, const char *port) {
    // Création du socket
    socketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFD < 0) {
        throw std::runtime_error("Erreur lors de la création du socket");
    }

    // Configuration de l'adresse du serveur
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(std::stoi(port));

    if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0) {
        close(socketFD);
        throw std::runtime_error("Adresse IP invalide ou inaccessible");
    }
}

// ** Constructeur pour le serveur **
SocketPosix::SocketPosix(const char *port) {
    // Création du socket
    socketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFD < 0) {
        throw std::runtime_error("Erreur lors de la création du socket");
    }

    // Configuration de l'adresse locale
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(std::stoi(port));

    // Liaison de l'adresse au socket
    if (bind(socketFD, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        close(socketFD);
        throw std::runtime_error("Erreur lors de la liaison (bind) du socket");
    }
}

// ** Destructeur **
SocketPosix::~SocketPosix() {
    if (socketFD >= 0) {
        close(socketFD);
    }
}

// ** Envoi d'un message au serveur **
void SocketPosix::sendToServer(const std::string &message) {
    ssize_t bytesSent = sendto(socketFD, message.c_str(), message.size(), 0, (sockaddr *)&serverAddr, sizeof(serverAddr));
    if (bytesSent < 0) {
        throw std::runtime_error("Erreur lors de l'envoi du message");
    }
}

// ** Réception d'un message du serveur **
void SocketPosix::receiveFromServer(std::string &message) {
    char buffer[DEFAULT_BUFLEN] = {0};
    socklen_t addrLen = sizeof(serverAddr);

    ssize_t bytesReceived = recvfrom(socketFD, buffer, DEFAULT_BUFLEN, 0, (sockaddr *)&serverAddr, &addrLen);
    if (bytesReceived < 0) {
        throw std::runtime_error("Erreur lors de la réception du message");
    }

    message = std::string(buffer, bytesReceived);
}

// ** Écoute (mode serveur) pour recevoir un message d'un client **
void SocketPosix::listen(std::string &message) {
    char buffer[DEFAULT_BUFLEN] = {0};
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Réception du message du client
    ssize_t bytesReceived = recvfrom(socketFD, buffer, DEFAULT_BUFLEN, 0, (sockaddr *)&clientAddr, &clientAddrLen);
    if (bytesReceived < 0) {
        throw std::runtime_error("Erreur lors de la réception du message client");
    }

    message = std::string(buffer, bytesReceived);

    // Réponse au client
    const char *ackMessage = "Message reçu avec succès";
    ssize_t bytesSent = sendto(socketFD, ackMessage, strlen(ackMessage), 0, (sockaddr *)&clientAddr, clientAddrLen);
    if (bytesSent < 0) {
        throw std::runtime_error("Erreur lors de l'envoi de l'accusé de réception");
    }
}
