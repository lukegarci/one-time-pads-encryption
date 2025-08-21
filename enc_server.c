#include "enc_server.h"

/*
 * Initializing the server struct
 */
void initServer(struct Server *server, int port) {
    server->port = port;
    server->is_running = 1;
    server->MAX_CONNECTIONS = 5;
    server->socket = -1;
    server->address = malloc(sizeof(struct sockaddr_in));
    server->size = sizeof(*(server->address));
}

/*
 * Free all dynamic memory of the struct Server
 */
void freeServer(struct Server *server) {
    free(server->address);
    free(server);
}

/*
 * Initializing the Client struct
 */
void initClient(struct Client *client) {
    client->socket = -1;
    client->recv_length = 0;
    client->data_length = 0;
    client->send_length = 0;
    client->index = 0;
    client->token = malloc(sizeof(char));
    client->buffer = malloc(sizeof(char) * 256);
    client->data_length_str = malloc(sizeof(char) * 16);
    client->address = malloc(sizeof(struct sockaddr_in));
    client->size = sizeof(*(client->address));
    memset(client->buffer, '\0', 256);
    memset(client->data_length_str, '\0', 16);
}

/*
 * Free all dynamic memory associated with the Client struct
 */
void freeClient(struct Client *client) {
    free(client->token);
    free(client->buffer);
    free(client->data_length_str);
    free(client->address);
    free(client);
}

/*
 * Set up the address struct for the server socket. Used from the example for Assignment5
 */
void setupAddressStruct(struct sockaddr_in *address, int port) {
    memset((char *)address, '\0', sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    address->sin_addr.s_addr = INADDR_ANY;
}

/*
 * This function handles the communication with a connected client. It receives
 * the data length, reads the data from the client, encrypts the data, and sends
 * the encrypted data back to the client.
 */
void processRequest(struct Client *client) {
    // Print a message indicating the server is connected to the client
    printf("Server: connected to client running at host %d port %d\n",
           ntohs(client->address->sin_addr.s_addr),
           ntohs(client->address->sin_port));
    // Receive the data length from the client
    client->recv_length = recv(client->socket, client->data_length_str, 16, 0);
    // Check if there was an error during receiving
    if (client->recv_length < 0) {
        error("Error: could not read from socket\n", -1);
    }
    // Convert the received data length string to an integer and double it
    client->data_length = string_to_integer(client->data_length_str) * 2;
    // Reallocate the buffer to accommodate the received data
    client->buffer = realloc(client->buffer, client->data_length + 1);
    // Receive the data from the client
    receive_data(client->socket, client->buffer, client->data_length);
    // Encrypt the data in the buffer
    encryptData(client);
    // Send the encrypted data back to the client
    send_data(client->socket, client->buffer, client->data_length / 2);
}


/*
 * This function iterates through the data and key in the buffer, performs
 * encryption by adding the corresponding characters, and stores the encrypted
 * characters back in the buffer.
 */
void encryptData(struct Client *client) {
    // Calculate the length of the data (half of the total data length)
    int length = client->data_length / 2;
    // Variables to store the current data character, key character, and encrypted character
    int data_ch = 0;
    int key_ch = 0;
    int enc_ch = 0;
    // Loop through the data and key in the buffer
    for (int i = 0; i < length; i++) {
        // Get the current data character
        data_ch = *(client->buffer + i);
        // Get the current key character
        key_ch = *(client->buffer + length + i);
        // If the data character is a space, set it to 91 (one past 'Z')
        if (data_ch == 32) {
            data_ch = 91;
        }
        // If the key character is a space, set it to 91 (one past 'Z')
        if (key_ch == 32) {
            key_ch = 91;
        }
        // Convert the characters to their corresponding positions in the alphabet
        data_ch -= 65;
        key_ch -= 65;
        // Perform the encryption by adding the data and key characters
        enc_ch = ((data_ch + key_ch) % 27) + 65;
        // If the encrypted character is 91, set it to a space
        if (enc_ch == 91) {
            enc_ch = 32;
        }
        // Store the encrypted character back in the buffer
        *(client->buffer + i) = enc_ch;
    }
}


/*
 * This function initializes the server, sets up the socket, binds it to the
 * specified port, and listens for incoming client connections. It handles
 * client requests in a loop until the server is stopped.
 */
int main(int argc, char **argv) {
    // Check if the correct number of arguments is provided
    if (argc < 2) {
        error("Error: expected one argument, port\n", 2);
    }
    // Convert the argument to an integer representing the port number
    int port = string_to_integer(argv[1]);
    // Allocate memory for the server struct
    struct Server *server = malloc(sizeof(struct Server));
    // Initialize the server struct
    initServer(server, port);
    // Create a socket for the server
    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    // Check if the socket could be opened
    if (server->socket < 0) {
        error("Error: socket could not be opened\n", 2);
    } else {
        // Set up the address struct for the server socket
        setupAddressStruct(server->address, server->port);
    }
    // Bind the socket to the specified port
    if (bind(server->socket, (struct sockaddr *)server->address, server->size) < 0) {
        error("Error: could not assign a name to the socket\n", 2);
    } else {
        // Listen for incoming connections with a maximum of MAX_CONNECTIONS
        listen(server->socket, server->MAX_CONNECTIONS);
    }
    // Loop to handle client connections
    while (server->is_running) {
        // Allocate memory for the client struct
        struct Client *client = malloc(sizeof(struct Client));
        // Initialize the client struct
        initClient(client);
        // Accept an incoming client connection
        client->socket = accept(server->socket, (struct sockaddr *)client->address, &client->size);
        // Check if the connection request was accepted
        if (client->socket < 0) {
            error("Error: could not accept the connection request\n", -1);
        } else {
            // Receive the token from the client
            client->recv_length = recv(client->socket, client->token, 1, 0);
            // Check if there was an error during receiving
            if (client->recv_length < 0) {
                error("Error: could not read from socket\n", -1);
            }
            // Check if the token indicates an encryption request
            if (*(client->token) == 'e') {
                // Set the token to indicate successful authentication
                *(client->token) = '1';
                // Send the authentication response to the client
                send(client->socket, client->token, 1, 0);
                // Create a new process to handle the client request
                pid_t pid = fork();
                switch (pid) {
                    case -1:
                        // Handle fork failure
                        error("Error: fork failed", 2);
                        break;
                    case 0:
                        // Process the client request in the child process
                        processRequest(client);
                        close(client->socket);
                        freeClient(client);
                        break;
                    default:
                        break;
                }
            } else {
                // Set the token to indicate authentication failure
                *(client->token) = '0';
                // Send the authentication response to the client
                send(client->socket, client->token, 1, 0);
                // Free the client struct
                freeClient(client);
            }
        }
    }
    // Close the server socket
    close(server->socket);
    // Free the server struct
    freeServer(server);
    return 0;
}
