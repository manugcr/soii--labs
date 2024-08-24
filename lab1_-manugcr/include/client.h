#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER 1024
#define TRUE 1

/**
 * @brief Check the return value of a function and exit if it is -1
 *
 * @param exp The return value of the function
 * @param msg The error message to print
 */
int check(int exp, const char* msg);
