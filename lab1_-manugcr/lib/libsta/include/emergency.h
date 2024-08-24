#pragma once

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define EMERGENCY_PIPE_PATH "/tmp/emergency_pipe"
#define TRUE 1
#define MAX_EMG_BUFFER 100
#define MAX_EMG_RANDOM_VALUE 31
#define FLOOR_EMG_VALUE 60


/**
 * @brief Start the emergencies process
 *
 * This function creates a random emergency every X seconds and sends it to the emergency pipe.
 * This emergency once received by the server, will trigger the server to shutdown and kick all clients.
 *
 * @return void
 */
void start_emergencies();

/**
 * @brief Open the emergency pipe
 *
 * This function opens the emergency pipe in write mode and returns the file descriptor.
 *
 * @return int The file descriptor of the emergency pipe
*/
int open_emergency_pipe();

/**
 * @brief Generate a random emergency message
 *
 * This function generates a random emergency message and stores it in the emergency_message buffer.
 *
 * @param emergency_message The buffer where the emergency message will be stored
 * @return void
*/
void generate_emergency_message(char *emergency_message);

/**
 * @brief Send an emergency message
 *
 * This function sends an emergency message to the emergency pipe.
 *
 * @param emergency_pipe_fd The file descriptor of the emergency pipe
 * @param emergency_message The emergency message to send
 * @return void
*/
void send_emergency_message(int emergency_pipe_fd, const char *emergency_message);

/**
 * @brief Sleep for a random interval
 *
 * This function sleeps for a random interval between 1 and 30 seconds.
 *
 * @return void
*/
void sleep_random_interval_emergency();

/**
 * @brief Start the emergencies process
 *
 * This function creates a random emergency every X seconds and sends it to the emergency pipe.
 *
 * @return void
*/
void start_emergencies();
