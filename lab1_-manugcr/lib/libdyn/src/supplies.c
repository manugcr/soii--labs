#include "supplies.h"

cJSON* state_summary;
char* json_string;

void create_state_summary()
{
    state_summary = cJSON_CreateObject();

    if (state_summary == NULL)
    {
        printf("[ERROR] Error creating JSON object\n");
        exit(1);
    }

    // Create and add the "alerts" object
    cJSON* alerts = cJSON_CreateObject();
    cJSON_AddItemToObject(state_summary, "alerts", alerts);

    // Add entries to the "alerts" object
    cJSON_AddNumberToObject(alerts, "north_entry", 0);
    cJSON_AddNumberToObject(alerts, "east_entry", 0);
    cJSON_AddNumberToObject(alerts, "west_entry", 0);
    cJSON_AddNumberToObject(alerts, "south_entry", 0);

    // Create and add the "supplies" object
    cJSON* supplies = cJSON_CreateObject();
    cJSON_AddItemToObject(state_summary, "supplies", supplies);

    // Create and add the "food" object under "supplies"
    cJSON* food = cJSON_CreateObject();
    cJSON_AddItemToObject(supplies, "food", food);

    // Add elements to the "food" object
    cJSON_AddNumberToObject(food, "meat", 0);
    cJSON_AddNumberToObject(food, "vegetables", 0);
    cJSON_AddNumberToObject(food, "fruits", 0);
    cJSON_AddNumberToObject(food, "water", 0);

    // Create and add the "medicine" object under "supplies"
    cJSON* medicine = cJSON_CreateObject();
    cJSON_AddItemToObject(supplies, "medicine", medicine);

    // Add elements to the "medicine" object
    cJSON_AddNumberToObject(medicine, "antibiotics", 0);
    cJSON_AddNumberToObject(medicine, "analgesics", 0);
    cJSON_AddNumberToObject(medicine, "bandages", 0);

    // Create and add the "emergency" object
    cJSON* emergency = cJSON_CreateObject();
    cJSON_AddItemToObject(state_summary, "emergency", emergency);

    // Add elements to the "emergency" object
    cJSON_AddStringToObject(emergency, "last_keepalived", "NULL");
    cJSON_AddStringToObject(emergency, "last_event", "NULL");
}

void modify_supplies(const char* message)
{
    char field[MAX_BUFFER];
    int value = 0;

    // Parse the message string
    if (sscanf(message, "%99s %d", field, &value) != 2)
    {
        printf("[ERROR] Invalid message format\n");
        return;
    }

    cJSON* element = NULL;
    cJSON* supplies = cJSON_GetObjectItem(state_summary, "supplies");

    // Check if the value is greater or equal to zero.
    if (value < 0)
    {
        printf("[ERROR] Invalid value: %d\n", value);
        return;
    }

    // Find the corresponding cJSON element based on the field name
    if (strcmp(field, "meat") == 0 || strcmp(field, "vegetables") == 0 || strcmp(field, "fruits") == 0 ||
        strcmp(field, "water") == 0)
    {
        cJSON* food = cJSON_GetObjectItem(supplies, "food");
        element = cJSON_GetObjectItem(food, field);
    }
    else if (strcmp(field, "antibiotics") == 0 || strcmp(field, "analgesics") == 0 || strcmp(field, "bandages") == 0)
    {
        cJSON* medicine = cJSON_GetObjectItem(supplies, "medicine");
        element = cJSON_GetObjectItem(medicine, field);
    }
    else
    {
        printf("[ERROR] Unknown field: %s\n", field);
        return;
    }

    // Update the value if the element is found
    if (element != NULL)
    {
        cJSON_SetNumberValue(element, value);
    }
    else
    {
        printf("[ERROR] Field not found: %s\n", field);
    }
}

void send_state_summary(int client_socket)
{
    // Generate the formatted JSON string with indentation
    char* formatted_json = cJSON_Print(state_summary);

    if (formatted_json == NULL)
    {
        printf("[ERROR] Error printing JSON object\n");
        return;
    }

    ssize_t bytes_sent = send(client_socket, formatted_json, strlen(formatted_json), 0);
    if (bytes_sent < 0)
    {
        perror("[ERROR] Error sending JSON data");
    }

    free(formatted_json);
}

void modify_alerts(const char* alert_message)
{
    // Parse the alert message to extract relevant information
    char area[MAX_BUFFER];
    float temperature = 0.0F;

    if (sscanf(alert_message, "Alert of possible infection in %99[^,], %f", area, &temperature) != 2)
    {
        printf("[ERROR] Invalid alert message format\n");
        return;
    }

    cJSON* alerts = cJSON_GetObjectItem(state_summary, "alerts");
    if (alerts == NULL)
    {
        printf("[ERROR] Error accessing alerts object\n");
        return;
    }

    // Find the corresponding alert entry based on the affected area
    cJSON* alert_entry = cJSON_GetObjectItem(alerts, area);
    if (alert_entry == NULL)
    {
        printf("[ERROR] Alert entry not found for area: %s\n", area);
        return;
    }

    // Update the alert entry value
    int current_value = (int)cJSON_GetNumberValue(alert_entry);
    cJSON_SetNumberValue(alert_entry, current_value + 1);
}

void modify_emergencies(const char* emergency_message)
{
    // Get the current time and date
    time_t raw_time;
    struct tm* time_info;
    char timestamp[MAX_BUFFER];

    time(&raw_time);
    if (raw_time == -1)
    {
        perror("[ERROR] time");
        return;
    }

    time_info = localtime(&raw_time);
    if (strftime(timestamp, MAX_BUFFER, "%c", time_info) == 0)
    {
        perror("[ERROR] strftime failed\n");
        return;
    }

    cJSON* emergency = cJSON_GetObjectItem(state_summary, "emergency");
    if (emergency == NULL)
    {
        printf("[ERROR] Error accessing emergency object\n");
        return;
    }

    cJSON* keepalived = cJSON_GetObjectItem(emergency, "last_keepalived");
    if (keepalived == NULL)
    {
        printf("[ERROR] Error accessing last_keepalived field\n");
        return;
    }
    cJSON_SetValuestring(keepalived, timestamp);

    cJSON* last_event = cJSON_GetObjectItem(emergency, "last_event");
    if (last_event == NULL)
    {
        printf("[ERROR] Error accessing last_event field\n");
        return;
    }
    cJSON_SetValuestring(last_event, emergency_message);
}
