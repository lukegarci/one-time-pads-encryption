#include "dec_client.h"


/*
 * Set up the address struct.
 */
void setupAddressStruct(struct sockaddr_in* address, int port, char* hostname) {
    struct hostent* host_info = gethostbyname(hostname);
    if (host_info == NULL) {
        error("Error: no such host\n", 2);
    } else {
        memset((char*) address, '\0', sizeof(*address));
        address->sin_family = AF_INET;
        address->sin_port = htons(port);
        memcpy((char*) &address->sin_addr.s_addr, host_info->h_addr_list[0],\
        host_info->h_length);
    }
}

/*
 * Initialize the client struct.
 */
void initClient(struct Client *client, int port) {
    client->port = port;
    client->socket = -1;
    client->data_length = 0;
    client->key_length = 0;
    client->index = 0;
    client->token = malloc(sizeof(char));
    client->buffer = malloc(sizeof(char) * 256);
    client->data_length_str = malloc(sizeof(char) * 16);
    client->server_address = malloc(sizeof(struct sockaddr_in));
    client->server_size = sizeof(*(client->server_address));
    *(client->token) = 'd';
    memset(client->buffer, '\0', 256);
    memset(client->data_length_str, '\0', 16);
}

/*
 * Free dynamically allocated memory in the client struct.
 */
void freeClient(struct Client *client) {
    free(client->token);
    free(client->buffer);
    free(client->data_length_str);
    free(client->server_address);
    free(client);
}


/*
 * This function reads characters from the specified data file and stores them
 * in the client's buffer. It handles valid characters
 * and dynamically reallocates the buffer as needed. The function
 * terminates when the end of the file or an invalid character is encountered.
 */
void readData(struct Client *client, char *filename) {
    // Flag to indicate if data is still being read
    int is_reading = 1;
    // Variable to keep track of the buffer size
    int count = 0;
    // Variable to store the current character
    char ch = '\0';
    // File pointer to open the file
    FILE *fp;
    // Open the file in read mode
    fp = fopen(filename, "r");
    // Loop to read characters from the file
    while (is_reading) {
        // Get the next character from the file
        ch = getc(fp);
        // Check if the character is a valid uppercase letter or space
        if ((ch >= 65 && ch <= 90) || ch == 32) {
            // Increment the data length and index
            client->data_length += 1;
            client->index += 1;
            // Store the character in the buffer
            *(client->buffer + client->index - 1) = ch;
            // Check if the buffer needs to be reallocated
            if (count < client->index) {
                count = client->index * 2;
                client->buffer = reallocate_string(client->buffer,\
                client->index, count);
            }
        }
            // Check if the end of the file or a newline character is encountered
        else if (ch == '\0' || ch == EOF || ch == '\n') {
            // Close the file
            fclose(fp);
            // Set the flag to stop reading
            is_reading = 0;
        }
            // Handle invalid characters
        else {
            // Close the file
            fclose(fp);
            // Print an error message and exit
            error("Error: invalid data\n", 2);
        }
    }
    // Reallocate the buffer to fit the exact data length
    client->buffer = reallocate_string(client->buffer, client->index, 1);
}



/*
 * This function reads characters from the specified key file and appends them
 * to the client's buffer. It handles valid characters
 * and dynamically reallocates the buffer as needed. The function
 * terminates when the end of the file or an invalid character is encountered,
 * or if the key length matches the data length.
 */
void readKey(struct Client *client, char *filename) {
    // Flag to indicate if data is still being read
    int is_reading = 1;
    // Flag to indicate if the end of the file or a termination condition is reached
    int is_term = 0;
    // Variable to keep track of the buffer size
    int count = 0;
    // Variable to store the current character
    char ch = '\0';
    // File pointer to open the file
    FILE *fp;
    // Open the file in read mode
    fp = fopen(filename, "r");
    // Loop to read characters from the file
    while (is_reading) {
        // Get the next character from the file
        ch = getc(fp);
        // Check if the character is a valid uppercase letter or space and the key length is less than or equal to the data length
        if (((ch >= 65 && ch <= 90) || ch == 32) &&\
        client->key_length <= client->data_length) {
            // Increment the key length and index
            client->key_length += 1;
            client->index += 1;
            // Store the character in the buffer
            *(client->buffer + client->index - 1) = ch;

            // Check if the buffer needs to be reallocated
            if (count < client->index) {
                count = client->index * 2;
                client->buffer = reallocate_string(client->buffer,\
                client->index, count);
            }
        }
            // Check if the end of the file, a newline, or a carriage return is encountered
        else if (ch == '\0' || ch == EOF || ch == '\n' || ch =='\r') {
            is_term = 1;
        }
            // Handle invalid characters
        else {
            // Close the file
            fclose(fp);
            // Print an error message and exit
            error("Error: invalid data\n", 2);
        }
        // Check if the key length matches the data length
        if (client->key_length == client->data_length) {
            is_term = 1;
        }
        // If a termination condition is reached
        if (is_term) {
            // Close the file
            fclose(fp);
            // Set the flag to stop reading
            is_reading = 0;
            // Check if the key length does not match the data length
            if (client->key_length != client->data_length) {
                error("Error: invalid key file length\n", 2);
            }
        }
    }
    // Reallocate the buffer to fit the exact data length
    client->buffer = reallocate_string(client->buffer, client->index, 1);
}

/*
 * This function initializes the client, sets up the socket, connects to the
 * server, reads the plaintext and key data, sends the data to the server, and
 * receives the encrypted data back from the server.
 */
int main(int argc, char **argv) {
    // Check if the correct number of arguments is provided
    if (argc < 4) {
        error("Error: expected three arguments, cypher, key, and port\n", 2);
    } else {
        // Get the plaintext, key, and port from the arguments
        char *plaintext = *(argv + 1);
        char *key = *(argv + 2);
        int port = string_to_integer(*(argv + 3));
        // Allocate memory for the client struct
        struct Client *client = malloc(sizeof(struct Client));
        // Initialize the client struct
        initClient(client, port);
        // Create a socket for the client
        client->socket = socket(AF_INET, SOCK_STREAM, 0);
        // Check if the socket could be opened
        if (client->socket < 0){
            error("Error: socket could not be opened\n", 2);
        } else {
            // Set up the address struct for the server socket
            setupAddressStruct(client->server_address, client->port, "localhost");
        }
        // Connect to the server
        if (connect(client->socket, (struct sockaddr *) client->server_address,\
        client->server_size) < 0) {
            error("Error: could not connect to server\n", 2);
        }
        // Read the plaintext data from the file
        readData(client, plaintext);
        // Read the key data from the file
        readKey(client, key);
        // Convert the data length to a string
        integer_to_string(client->data_length, client->data_length_str);
        // Send the token to the server
        send_data(client->socket, client->token, 1);
        // Receive the token from the server
        receive_data(client->socket, client->token, 1);
        // Check if the authentication was successful
        if (*(client->token) == '1') {
            // Send the data length and buffer to the server
            send_data(client->socket, client->data_length_str, 16);
            send_data(client->socket, client->buffer, string_length(client->buffer));
            // Receive the encrypted data from the server
            receive_data(client->socket, client->buffer, client->data_length);
            // Print the encrypted data
            write(1, client->buffer, client->data_length);
            write(1, "\n", 1);
        } else {
            error("Error: authentication failed\n", -1);
        }
        // Close the socket and free the client struct
        close(client->socket);
        freeClient(client);
    }
    return 0;
}
