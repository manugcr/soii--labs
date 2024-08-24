#pragma once

#include <arpa/inet.h>
#include <cJSON.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_PENDING_CONNECTIONS 5
#define MAX_CLIENTS 100
#define MAX_BUFFER 1024
#define MAX_LOG_BUFFER 1072
#define FIFO_PERMISSION 0666
#define SMO_SIZE 4096
#define SMO_NAME "/state_summary"
#define ALERT_PIPE_PATH "/tmp/alert_pipe"
#define EMERGENCY_PIPE_PATH "/tmp/emergency_pipe"
#define TRUE 1

/* Global structs for server config */
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

/* Data structure to keep track of client, role and num of clientes online */
typedef struct
{
    int socket;
    int role;
} ClientInfo;
ClientInfo connected_clients[MAX_CLIENTS];
int num_connected_clients = 0;

int alert_pipe_fd;
int emergency_pipe_fd;

/**
 * @brief Setup the server TCP IPv4 socket and start listening on the specified port
 *
 * @param PORT The port number to listen on
 * @param backlog The maximum number of pending connections
 * @return int The server socket file descriptor
 */
int setup_server(int PORT, int backlog);

/**
 * @brief Accept a new connection on the server
 *
 * The function will accept a new connection on the server socket and authenticate the client.
 * If the client is authenticated, the client socket will be added to the connected_clients array.
 * If not authenticated, the client will be disconnected.
 *
 * @param server_socket The server socket file descriptor
 * @return int The client socket file descriptor
 */
int accept_new_connection(int server_socket);

/**
 * @brief Authenticate the client
 *
 * The function will read the credentials from the client and check if they are correct.
 * If the credentials are root, the client will be authenticated and the function will return 1.
 * If the credentials are hostname, the client will be authenticated and the function will return 0.
 * If the credentials are incorrect, the client will be disconnected and the function will return -1.
 *
 * @param client_socket The client socket file descriptor
 * @return int The role of the client
 */
int auth_client(int client_socket);

/**
 * @brief Check the return value of a function and exit if it is -1
 *
 * @param exp The return value of the function
 * @param msg The error message to print
 */
int check(int exp, const char* msg);

/**
 * @brief Log an activity to the server.log file
 *
 * The function will log the activity to the server.log file
 * with a format of "timestamp, activity".
 *
 * @param activity The activity to log
 * @return void
 */
void log_activity(const char* activity);

/**
 * @brief Get the address of the client associated with the client socket
 *
 * The function will retrieve the address information associated with the client socket
 * and format it into the provided buffer to use it in the log.
 *
 * @param client_socket The client socket file descriptor
 * @param client_address The buffer to store the client address
 * @param buffer_size The size of the buffer
 * @return void
 */
void get_client_address(int client_socket, char* client_address, size_t buffer_size);

/**
 * @brief Disconnect a client from the server
 *
 * The function will disconnect a client from the server and remove it from the connected_clients array.
 *
 * @param client_socket The client socket file descriptor
 * @param i The index of the client in the connected_clients array

*/
void disconnect_client(int client_socket, int client_index);

/**
 * @brief Handle alerts from the alert pipe
 *
 * The function reads alerts from the alert pipe and sends them to all connected clients.
 *
 * @param alert_pipe_fd The alert pipe file descriptor
 * @return void
 */
void handle_alerts(int alert_pipe_fd);

/**
 * @brief Handle emergencies from the emergency pipe
 *
 * The function reads emergencies from the emergency pipe and sends them to all connected clients.
 *
 * @param emergency_pipe_fd The emergency pipe file descriptor
 * @return void
 */
void handle_emergencies(int emergency_pipe_fd);

/**
 * @brief Disconnect all clients from the server
 *
 * The function will disconnect all clients from the server and remove them from the connected_clients array.
 *
 * @return void
 */
void disconnect_all_clients();

/**
 * @brief Setup the shared memory for the state summary
 *
 * The function creates a shared memory object and maps
 * it into the address space of the calling process.
 *
 * @return void
 */
void setup_shared_memory();
