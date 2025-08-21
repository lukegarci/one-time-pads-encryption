#include "keygen.h"
#include "utility.h"


/*
 * This function takes a single command-line argument representing the length
 * of the string to be generated. It generates a random string of uppercase
 * letters and spaces, and prints it to the standard output.
 */
int main(int argc, char **argv) {
    // Check if the correct number of arguments is provided
    if (argc == 2) {
        // Convert the argument to an integer representing the size of the string
        int size = string_to_integer(*(argv + 1));
        // Variable to store the randomly generated character
        int ch = 0;
        // Seed the random number
        srand(time(NULL));
        // Loop to generate the random string
        for (int i = 0; i < size; i++) {
            // Generate a random number between 0 and 26
            ch = rand() % (27);
            // If the random number is 26, set the character to a space
            if (ch == 26) {
                ch = 32;
            }
                // Otherwise, set the character to an uppercase letter
            else {
                ch += 65;
            }
            // Print the generated character
            printf("%c", ch);
        }
        // Print a newline character at the end of the string
        printf("\n");
    }
        // If the incorrect number of arguments is provided, print an error message
    else {
        fprintf(stderr, "Error: expected one argument\n");
    }
    return 0;
}
