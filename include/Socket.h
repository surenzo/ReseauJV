#pragma once

#include <string>

class Socket {
private:
    int sendSocket;              
    int listenSocket;             
    struct sockaddr serverAddr;   
    socklen_t serverAddrLen;      

    struct sockaddr clientAddr;   
    socklen_t clientAddrLen;     

    static constexpr int DEFAULT_BUFLEN = 512; 
public:
    Socket(const char* address, const char* port); 
    Socket(const char* port);                     
    ~Socket();

    void sendToServer(const char* message);
    void receiveFromServer(char* buffer);
    void listen(char* buffer);
};
