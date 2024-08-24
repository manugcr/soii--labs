#include "emergency.h"

int open_emergency_pipe()
{
    int emergency_pipe_fd = open(EMERGENCY_PIPE_PATH, __O_CLOEXEC | O_WRONLY);
    if (emergency_pipe_fd == -1)
    {
        perror("[ERROR] Failed to open emergency pipe");
        exit(EXIT_FAILURE);
    }
    return emergency_pipe_fd;
}

void generate_emergency_message(char* emergency_message)
{
    int sprintf_result = sprintf(emergency_message, "Server failure/Server shutdown\n");
    if (sprintf_result < 0)
    {
        perror("[ERROR] sprintf");
        exit(EXIT_FAILURE);
    }
}

void send_emergency_message(int emergency_pipe_fd, const char* emergency_message)
{
    ssize_t bytes_written = write(emergency_pipe_fd, emergency_message, strlen(emergency_message));
    if (bytes_written == -1) {
        perror("[ERROR] write");
        exit(EXIT_FAILURE);
    }
}

void sleep_random_interval_emergency()
{
    sleep((rand() % MAX_EMG_RANDOM_VALUE) + FLOOR_EMG_VALUE); // {60 to 90} seconds
}

void start_emergencies()
{
    int emergency_pipe_fd = open_emergency_pipe();
    char emergency_message[MAX_EMG_BUFFER];

    srand(time(NULL));

    #pragma unroll
    while (TRUE)
    {
        sleep_random_interval_emergency();

        generate_emergency_message(emergency_message);

        send_emergency_message(emergency_pipe_fd, emergency_message);

        memset(emergency_message, 0, sizeof(emergency_message));
    }

    close(emergency_pipe_fd);
}
