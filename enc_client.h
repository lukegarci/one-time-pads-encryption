#ifndef ENC_CLIENT_H
#define ENC_CLIENT_H

#include "utility.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


struct Client {
    int port;
    int socket;
    int data_length;
    int key_length;
    int index;
    char *token;
    char *buffer;
    char *data_length_str;
    struct sockaddr_in *server_address;
    socklen_t server_size;
};

// Function Declarations
void setupAddressStruct(struct sockaddr_in* address, int port, char* hostname);
void readFile(struct Client *client, char *filename);
void readKey(struct Client *client, char *filename);

#endif