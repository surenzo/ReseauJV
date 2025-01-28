#ifndef SOCKET_H
#define SOCKET_H


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>


class Socket {
private:
    int SendSocket;
    int ListenSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t serverAddrLen, clientAddrLen;
    
public:
    Socket(const char* ip, const char* port);
    Socket(const char* port);
    ~Socket();
    void sendToServer(const char* message);
    void receiveFromServer(char* recvbuf);
    void listen(char* port);

    virtual void sendToServer(const std::string &message) = 0;
    virtual void receiveFromServer(std::string &message) = 0;

    virtual void listen(std::string &message) = 0;
};


#endif
