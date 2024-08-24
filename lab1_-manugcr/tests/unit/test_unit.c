#include "alerts.h"
#include "emergency.h"
#include "supplies.h"
#include <cJSON.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unity.h>

/********************************************************************/
/*                      Tests for emergency.c                       */
/********************************************************************/
void test_generate_emergency_message(void)
{
    char emergency_message[MAX_EMG_BUFFER];
    generate_emergency_message(emergency_message);
    // Ensure the generated message is not null
    TEST_ASSERT_NOT_NULL(emergency_message);
    // Ensure the generated message is not empty
    TEST_ASSERT_TRUE(strlen(emergency_message) > 0);
}

/********************************************************************/
/*                      Tests for alerts.c                          */
/********************************************************************/

void test_generate_alert(void)
{
    char alert_message[MAX_AL_BUFFER];
    const char* entries[] = {"north_entry", "east_entry", "west_entry", "south_entry"};

    generate_alert(alert_message, entries);

    // Add your assertions here
    TEST_ASSERT_NOT_NULL(alert_message);
    TEST_ASSERT_TRUE(strlen(alert_message) > 0);
}

void test_send_alert_if_needed(void)
{
    // Provide a mock alert pipe file descriptor
    int alert_pipe_fd = 1;

    // Provide a mock alert message and temperature
    const char* alert_message = "Mock alert message";
    float temperature = 40.0; // Above the allowed temperature

    // Call the function to send alert if needed
    send_alert_if_needed(alert_pipe_fd, alert_message, temperature);

    // As this is a void function, we can only ensure it doesn't crash
    // If you have additional logic inside this function, you can add more assertions accordingly
}

void test_generate_random_temperature(void)
{
    float temperature = generate_random_temperature();

    // Add your assertions here
    TEST_ASSERT_TRUE(temperature >= MIN_TEMPERATURE);
    TEST_ASSERT_TRUE(temperature <= MAX_TEMPERATURE);
}

/********************************************************************/
/*                      Tests for supplies.c                        */
/********************************************************************/
void reset_state_summary(void)
{
    if (state_summary != NULL)
    {
        cJSON_Delete(state_summary);
    }
    create_state_summary();
}

// Tests for create_state_summary
void test_create_state_summary(void)
{
    create_state_summary();
    TEST_ASSERT_NOT_NULL(state_summary);

    cJSON* alerts = cJSON_GetObjectItem(state_summary, "alerts");
    TEST_ASSERT_NOT_NULL(alerts);

    cJSON* supplies = cJSON_GetObjectItem(state_summary, "supplies");
    TEST_ASSERT_NOT_NULL(supplies);

    cJSON* emergency = cJSON_GetObjectItem(state_summary, "emergency");
    TEST_ASSERT_NOT_NULL(emergency);
}

// Tests for modify_supplies
void test_modify_supplies(void)
{
    reset_state_summary();

    modify_supplies("meat 10");
    cJSON* food = cJSON_GetObjectItem(cJSON_GetObjectItem(state_summary, "supplies"), "food");
    TEST_ASSERT_EQUAL(10, cJSON_GetObjectItem(food, "meat")->valueint);

    modify_supplies("water 5");
    TEST_ASSERT_EQUAL(5, cJSON_GetObjectItem(food, "water")->valueint);

    modify_supplies("antibiotics 3");
    cJSON* medicine = cJSON_GetObjectItem(cJSON_GetObjectItem(state_summary, "supplies"), "medicine");
    TEST_ASSERT_EQUAL(3, cJSON_GetObjectItem(medicine, "antibiotics")->valueint);
}

// Mock send function
ssize_t mock_send(int sockfd, const void* buf, size_t len, int flags)
{
    return len;
}

// Tests for send_state_summary
void test_send_state_summary(void)
{
    int client_socket = 0; // Mock socket descriptor
    reset_state_summary();

// Mock send function with macro
#define send(sockfd, buf, len, flags) mock_send(sockfd, buf, len, flags)

    send_state_summary(client_socket);

// Remove the macro substitution to avoid affecting other tests
#undef send
}

// Tests for modify_alerts
void test_modify_alerts(void)
{
    reset_state_summary();

    modify_alerts("Alert of possible infection in north_entry, 39.0");
    cJSON* alerts = cJSON_GetObjectItem(state_summary, "alerts");
    TEST_ASSERT_EQUAL(1, cJSON_GetObjectItem(alerts, "north_entry")->valueint);

    modify_alerts("Alert of possible infection in north_entry, 39.0");
    TEST_ASSERT_EQUAL(2, cJSON_GetObjectItem(alerts, "north_entry")->valueint);
}

// Tests for modify_emergencies
void test_modify_emergencies(void)
{
    reset_state_summary();

    modify_emergencies("Test emergency message");

    cJSON* emergency = cJSON_GetObjectItem(state_summary, "emergency");
    cJSON* last_event = cJSON_GetObjectItem(emergency, "last_event");
    TEST_ASSERT_EQUAL_STRING("Test emergency message", last_event->valuestring);

    // Assuming modify_emergencies updates the last_keepalived field with a valid timestamp
    cJSON* last_keepalived = cJSON_GetObjectItem(emergency, "last_keepalived");
    TEST_ASSERT_NOT_EQUAL(NULL, last_keepalived->valuestring);
}

void setUp(void)
{
    reset_state_summary();
}

void tearDown(void)
{
    if (state_summary != NULL)
    {
        cJSON_Delete(state_summary);
        state_summary = NULL;
    }
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_generate_emergency_message);

    RUN_TEST(test_generate_alert);
    RUN_TEST(test_send_alert_if_needed);
    RUN_TEST(test_generate_random_temperature);

    RUN_TEST(test_create_state_summary);
    RUN_TEST(test_modify_supplies);
    RUN_TEST(test_send_state_summary);
    RUN_TEST(test_modify_alerts);
    RUN_TEST(test_modify_emergencies);

    return UNITY_END();
}
