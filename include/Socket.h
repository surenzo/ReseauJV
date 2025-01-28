
#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <arpa/inet.h>

class Socket {
public:
    // Constructeur et destructeur
    Socket(const char* ip, const char* port);
    ~Socket();

    static const char* DEFAULT_PORT;

    // Méthodes de communication
    void sendToServer(const char* message);
    void receiveFromServer(char* recvbuf);
    void listen(char* buffer);

    // Méthodes pour définir les adresses
    void setServerAddr(const char* ip, const char* port);
    void setListenAddr(const char* port);

    // Méthode pour commencer à écouter
    void startListening();

private:
    // Sockets pour envoyer et écouter
    int SendSocket;         // Socket pour l'envoi de données
    int ListenSocket;       // Socket pour l'écoute des connexions

    // Structures pour les adresses
    struct sockaddr_in serverAddr;   // Adresse du serveur pour l'envoi
    struct sockaddr_in sender_addr;  // Adresse de l'expéditeur pour la réception
    struct sockaddr_in clientAddr;   // Adresse du client pour l'écoute
    struct sockaddr* result;         // Pointeur générique pour sockaddr
    socklen_t result_size;           // Taille de la structure sockaddr
    socklen_t sender_addr_size;      // Taille de la structure de l'expéditeur
    socklen_t clientAddrLen;         // Taille de la structure du client
    

};

#endif 
