#ifndef UTIL_H
#define UTIL_H

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// Function Declarations
void error(char *message, int status);
int raise_to_power(int operand, int power);
int random_integer(int max);
void integer_to_string(int num, char *string);
int string_to_integer(char *string);
int string_length(char *string);
char *reallocate_string(char *buffer, int length, int count);
void get_user_input(char *prompt, char *buffer, int size);
void send_data(int socket, char *buffer, int count);
void receive_data(int socket, char *buffer, int count);

#endif