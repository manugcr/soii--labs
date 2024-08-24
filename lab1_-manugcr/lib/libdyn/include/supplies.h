#pragma once

#include <cJSON.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_BUFFER 1024

extern cJSON* state_summary;
extern char* json_string;

/**
 * @brief Create the state summary JSON object
 *
 * This function creates the state summary JSON object with a predefined structure.
 * The json helps to keep track of the supplies, alerts, and emergency events.
 *
 * @return void
 */
void create_state_summary();

/**
 * @brief Modify the supplies based on the message received
 *
 * This function modifies the supplies based on the message received from the client.
 * The message format is "<field> <value>", where field is the name of the supply field
 * and value is the new value to be set.
 *
 * @param message The message received from the client
 * @return void
 *
 */
void modify_supplies(const char* message);

/**
 * @brief Modify the alerts based on the alert message received
 *
 * This function modifies the alerts based on the alert message received from the client.
 * The message format is "Alert of possible infection in <area>, <temperature>", where area
 * is the affected area and temperature is the temperature value.
 *
 * @param alert_message The alert message received from the client
 * @return void
 */
void modify_alerts(const char* message);

/**
 * @brief Modify the emergency events based on the event message received
 *
 * This function modifies the emergency events based on the event message received from the client.
 * The message format is "<event_message>", where event_message is the description of the emergency event.
 *
 * @param event_message The event message received from the client
 * @return void
 */
void modify_emergencies(const char* emergency_message);

/**
 * @brief Send the state summary JSON object to the client
 *
 * @return void
 */
void send_state_summary(int client_socket);
