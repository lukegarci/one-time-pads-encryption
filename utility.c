#include "utility.h"


/*
 * This function prints the specified error message to the standard error
 * stream and exits the program with the specified status code.
 */
void error(char *message, int status) {
    // Print the error message to the standard error stream
    fprintf(stderr,"%s", message);

    // Check if the status code is non-negative
    if (status >= 0) {
        // Exit the program with the specified status code
        exit(status);
    }
}

/*
 * This function calculates the result of raising the operand to the specified
 * power and returns the result.
 */
int raise_to_power(int operand, int power) {
    // Initialize the result to store the final value
    int result = 0;
    // Loop to calculate the power
    for (int i = 0; i < power + 1; i++) {
        // If the current power is 0, set the result to 1
        if (i == 0) {
            result = 1;
        }
            // Otherwise, multiply the result by the operand
        else {
            result *= operand;
        }
    }
    // Return the final result
    return result;
}

/*
 * This function seeds the random number generator with the current time and
 * generates a random integer between 0 and the specified maximum value.
 */
int random_integer(int max) {
    // Variable to store the random integer
    int result;
    // Variable to store the current time
    time_t t;
    // Seed the random number generator with the current time
    srand((unsigned) time(&t));
    srand(time(NULL));
    // Generate a random integer between 0 and the specified maximum value
    result = rand() % (max + 1);
    // Return the random integer
    return result;
}

/*
 * This function converts the specified integer to a string and stores it in
 * the provided buffer. It handles negative numbers and adds a null terminator
 * at the end.
 */
void integer_to_string(int num, char *string) {
    // Initialize the index to keep track of the current position in the string
    int index = 0;
    // Flag to indicate if the number is negative
    int is_negative = 0;
    // Variable to store the current character
    char temp = 0;
    // Check if the number is negative
    if (num < 0) {
        is_negative = 1;
        num *= -1;
    }
    // Loop to convert the integer to a string
    while (num != 0) {
        index++;
        *(string + index - 1) = (num % 10) + 48;
        num /= 10;
    }
    // If the number is negative, add the negative sign to the string
    if (is_negative) {
        index++;
        *(string + index - 1) = '-';
    }
    // Reverse the string to get the correct order
    for (int i = 0; i < index / 2; i++) {
        temp = *(string + i);
        *(string + i) = *(string + index - 1 - i);
        *(string + index - 1 - i) = temp;
    }
    // Add a null terminator at the end of the string
    index++;
    *(string + index - 1) = '\0';
}


/*
 * This function iterates through the string, converts each character to its
 * corresponding digit, and accumulates the result. It handles negative numbers
 * and checks for invalid characters.
 */
int string_to_integer(char *string) {
    // Initialize the result to store the final integer value
    int result = 0;
    // Flag to indicate if the string contains only digits
    int is_digit = 1;
    // Flag to indicate if the number is negative
    int is_negative = 0;
    // Variable to store the current character
    int ch = -1;
    // Variable to keep track of the current position in the string
    int position = 0;
    // Loop through the string from the end to the beginning
    for (int i = string_length(string) - 1; i > -1 ; i--) {
        // Get the current character
        ch = *(string + i);
        // Check if the current character is a negative sign
        if (i == 0 && ch == 45) {
            is_negative = 1;
        }
            // Check if the current character is a digit
        else if (ch >= 48 && ch <= 57) {
            // Convert the character to its corresponding digit
            ch -= 48;
            // Accumulate the result by adding the digit multiplied by the power of 10
            result += ch * raise_to_power(10, position);
            // Increment the position for the next digit
            position += 1;
        }
            // Handle invalid characters
        else {
            result = 0;
            is_digit = 0;
            break;
        }
    }
    // If the string contains only digits and is negative, negate the result
    if (is_digit && is_negative) {
        result *= -1;
    }
    // Return the final integer value
    return result;
}

/*
 * This function iterates through the string until it encounters the null
 * terminator, counting the number of characters in the string.
 */
int string_length(char *str) {
    // Initialize the index to keep track of the current position in the string
    int index = 0;
    // Loop through the string until the null terminator is encountered
    while (*(str + index) != '\0') {
        // Increment the index to move to the next character
        index++;
    }
    // Return the length of the string
    return index;
}



/*
 * This function creates a temporary copy of the current buffer, reallocates
 * the buffer to a new size, copies the data back from the temporary buffer,
 * and adds a null terminator at the end.
 */
char *reallocate_string(char *buffer, int length, int count) {
    // Create a temporary array to store the current buffer contents
    char temp[length];
    // Copy the current buffer contents to the temporary array
    for (int i = 0; i < length; i++) {
        temp[i] = *(buffer + i);
    }
    // Reallocate the buffer to the new size
    buffer = realloc(buffer, sizeof(char) * (length + count));
    // Copy the data back from the temporary array to the reallocated buffer
    for (int i = 0; i < length; i++) {
        *(buffer + i) = temp[i];
    }
    // Add a null terminator at the end of the buffer
    *(buffer + length) = '\0';
    // Return the reallocated buffer
    return buffer;
}


/*
 * This function displays a prompt, reads a line of input from the user, and
 * stores it in the buffer. The input is truncated to fit the buffer size, and
 * a null terminator is added at the end.
 */
void get_user_input(char *prompt, char *buffer, int size) {
    // Display the prompt
    printf("%s", prompt);
    // Flush the output buffer to ensure the prompt is displayed
    fflush(stdout);
    // Read a line of input from the user
    fgets(buffer, size, stdin);
    // Loop through the buffer to find the newline character or the end of the buffer
    for (int i = 0; i < size; i++) {
        // Check if the current character is a newline or the end of the buffer
        if (*(buffer + i) == '\n' || i == size - 1) {
            // Replace the newline character with a null terminator
            *(buffer + i) = '\0';
            break;
        }
    }
}


/*
 * This function continuously sends data from the buffer through the specified
 * socket until the specified number of bytes is sent.
 */
void send_data(int socket, char *buffer, int count) {
    // Flag to indicate if data is still being sent
    int is_sending = 1;
    // Index to keep track of the current position in the buffer
    int index = 0;
    // Loop until all data is sent
    while (is_sending) {
        // Send data through the socket and update the index
        index += send(socket, buffer + index, count - index, 0);
        // Check if the required number of bytes has been sent
        if (index == count) {
            // Set the flag to stop sending data
            is_sending = 0;
        } else if (index < 0) {
            // Check if there was an error during sending
            error("Error: could not write to socket\n", 2);
        }
    }
}

/*
 * This function continuously reads data from the specified socket until the
 * specified number of bytes is received. The received data is stored
 * in the buffer, and a null terminator is added at the end.
 */
void receive_data(int socket, char *buffer, int count) {
    // Flag to indicate if data is still being received
    int is_receiving = 1;
    // Index to keep track of the current position in the buffer
    int index = 0;
    // Loop until all data is received
    while (is_receiving) {
        // Receive data from the socket and update the index
        index += recv(socket, buffer + index, count - index, 0);
        // Check if the required number of bytes has been received
        if (index == count) {
            // Add a null terminator at the end of the buffer
            *(buffer + index) = '\0';
            // Set the flag to stop receiving data
            is_receiving = 0;
        } else if (index < 0) {
            // Check if there was an error during receiving
            error("Error: could not read from socket\n", 2);
        }
    }
}
