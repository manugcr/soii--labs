#pragma once

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define ALERT_PIPE_PATH "/tmp/alert_pipe"
#define ALLOWED_TEMPERATURE 38.0
#define MIN_TEMPERATURE 35.0f
#define MAX_TEMPERATURE 42.0f
#define TRUE 1
#define MAX_AL_BUFFER 100
#define MAX_ALERTS_RANDOM_VALUE 16
#define FLOOR_AL_VALUE 10

/**
 * @brief Start the alerts process
 *
 * This function creates a random alert every X seconds with a format of
 * "Alert of possible infection in <entry>, <temperature>\n" and sends it to the alert pipe.
 * If the temperature is greater than or equal to MAX_TEMPERATURE, the alert is sent.
 *
 * @return void
 */
void start_alerts();

/**
 * @brief Open the alert pipe
 *
 * This function opens the alert pipe in write mode and returns the file descriptor.
 *
 * @return int The file descriptor of the alert pipe
*/
int open_alert_pipe();

/**
 * @brief Generate a random alert message
 *
 * This function generates a random alert message and stores it in the alert_message buffer.
 *
 * @param alert_message The buffer where the alert message will be stored
 * @param entries The entries to choose from
 * @return void
*/
void generate_alert(char *alert_message, const char *entries[]);

/**
 * @brief Send an alert message
 *
 * This function sends an alert message to the alert pipe.
 *
 * @param alert_pipe_fd The file descriptor of the alert pipe
 * @param alert_message The alert message to send
 * @param temperature The temperature to send
 * @return void
*/
void send_alert_if_needed(int alert_pipe_fd, const char *alert_message, float temperature);

/**
 * @brief Generate a random temperature
 *
 * This function generates a random temperature between MIN_TEMPERATURE and MAX_TEMPERATURE.
 *
 * @return float The generated temperature
*/
float generate_random_temperature();

/**
 * @brief Sleep for a random interval
 *
 * This function sleeps for a random interval between 1 and 15 seconds.
 *
 * @return void
*/
void sleep_random_interval();
