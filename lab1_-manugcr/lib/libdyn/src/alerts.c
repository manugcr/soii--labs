#include "alerts.h"

void start_alerts()
{
    int alert_pipe_fd = open_alert_pipe();
    char alert_message[MAX_AL_BUFFER];
    const char* entries[] = {"north_entry", "east_entry", "west_entry", "south_entry"};

    srand(time(NULL));

    #pragma unroll
    while (TRUE)
    {
        // Sleep for a random interval before sending the next alert
        memset(alert_message, 0, sizeof(alert_message));
        sleep_random_interval();

        generate_alert(alert_message, entries);
        float temperature = generate_random_temperature();

        send_alert_if_needed(alert_pipe_fd, alert_message, temperature);

        memset(alert_message, 0, sizeof(alert_message));
    }

    close(alert_pipe_fd);
}

int open_alert_pipe()
{
    int alert_pipe_fd = open(ALERT_PIPE_PATH, __O_CLOEXEC | O_WRONLY);
    if (alert_pipe_fd == -1)
    {
        perror("Failed to open alert pipe");
        exit(EXIT_FAILURE);
    }
    return alert_pipe_fd;
}

void generate_alert(char* alert_message, const char* entries[])
{
    int entry_index = rand() % 4;
    float temperature = generate_random_temperature();

    int sprintf_result =
        sprintf(alert_message, "Alert of possible infection in %s, %.1f\n", entries[entry_index], temperature);
    if (sprintf_result < 0)
    {
        perror("[ERROR] sprintf error at alerts");
        exit(EXIT_FAILURE);
    }
}

void send_alert_if_needed(int alert_pipe_fd, const char* alert_message, float temperature)
{
    if (temperature >= ALLOWED_TEMPERATURE)
    {
        ssize_t bytes_written = write(alert_pipe_fd, alert_message, strlen(alert_message));
        if (bytes_written == -1) {
            perror("[ERROR] write");
            exit(EXIT_FAILURE);
        }
    }
}

float generate_random_temperature()
{
    return MIN_TEMPERATURE + ((float)rand() / RAND_MAX) * (MAX_TEMPERATURE - MIN_TEMPERATURE);
}

void sleep_random_interval()
{
    sleep((rand() % MAX_ALERTS_RANDOM_VALUE) + FLOOR_AL_VALUE); // {10 to 25} seconds
}
