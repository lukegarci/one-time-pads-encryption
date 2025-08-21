#ifndef ENC_SERVER_H
#define ENC_SERVER_H

#include "utility.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>



struct Server {
    int MAX_CONNECTIONS;
    int port;
    int is_running;
    int socket;
    struct sockaddr_in *address;
    socklen_t size;
};

struct Client {
    int socket;
    int recv_length;
    int send_length;
    int data_length;
    int index;
    char *token;
    char *buffer;
    char *data_length_str;
    struct sockaddr_in *address;
    socklen_t size;
};

//Function Declarations
void initServer(struct Server *server, int port);
void freeServer(struct Server *server);
void initClient(struct Client *client);
void freeClient(struct Client *client);
void setupAddressStruct(struct sockaddr_in* address, int port);
void processRequest(struct Client *client);
void encryptData(struct Client *client);

#endif