#include "server.h"
#include "alerts.h"
#include "emergency.h"
#include "supplies.h"

int main(int argc, char const* argv[])
{
    fd_set read_fds;
    int PORT = 0;
    int server_socket = 0;
    int client_socket = 0;
    char buffer[MAX_BUFFER];
    char log_buffer[MAX_LOG_BUFFER];
    char client_address[MAX_BUFFER];

    /* Get the port from the arguments and start the server */
    if (argc != 2)
    {
        printf("Usage: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    PORT = atoi(argv[1]);

    /* Start server sockets and pipes for alerts and emergencies */
    server_socket = setup_server(PORT, MAX_PENDING_CONNECTIONS);

    /* Start a shared memory block to manage the supplies json module */
    setup_shared_memory();

    /**
     * Main server loop
     * - Wait for activity on the server socket
     * - If activity is on the server socket, accept new connection
     * - If activity is on a client socket, read the message
     * - If client disconnects, remove it from the connected_clients data structure
     * - If an alert is received log it.
     * - If an emergency is received disconnect all clients.
     */
    #pragma unroll
    while (TRUE)
    {
        ssize_t valread = 0;
        int max_socket_fd = 0;

        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        FD_SET(alert_pipe_fd, &read_fds);
        FD_SET(emergency_pipe_fd, &read_fds);
        max_socket_fd = (server_socket > alert_pipe_fd) ? server_socket : alert_pipe_fd;
        max_socket_fd = (max_socket_fd > emergency_pipe_fd) ? max_socket_fd : emergency_pipe_fd;

        /* Add client sockets to the set if its valid. */
        #pragma unroll
        for (int i = 0; i < max_socket_fd; i++)
        {
            client_socket = connected_clients[i].socket;
            if (client_socket > 0)
            {
                FD_SET(client_socket, &read_fds);
            }

            if (client_socket > max_socket_fd)
            {
                max_socket_fd = client_socket;
            }
        }

        /* Wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely */
        if ((select(max_socket_fd + 1, &read_fds, NULL, NULL, NULL) < 0) && (errno != EINTR))
        {
            perror("[ERROR] Select failed");
            exit(EXIT_FAILURE);
        }

        /* If something happened on the server socket , then its an incoming connection. */
        if (FD_ISSET(server_socket, &read_fds))
        {
            if (num_connected_clients < MAX_CLIENTS)
            {
                client_socket = accept_new_connection(server_socket);
            }
            else
            {
                printf("[SERVER] Maximum number of clients reached. Unable to accept new connections.\n");
                close(client_socket);
            }
        }

        /* If something happend on the client sockets, its either a message or a disconnect */
        #pragma unroll
        for (int i = 0; i < max_socket_fd; i++)
        {
            client_socket = connected_clients[i].socket;
            if (client_socket > 0 && FD_ISSET(client_socket, &read_fds))
            {
                memset(buffer, 0, MAX_BUFFER);
                valread = read(client_socket, buffer, MAX_BUFFER);
                if (valread == 0)
                {
                    disconnect_client(client_socket, i);
                }
                else
                {
                    /* Grabd the command sent by the user */
                    char command[MAX_BUFFER];
                    int sscanf_result = sscanf(buffer, "%99s", command);
                    if (sscanf_result == EOF)
                    {
                        perror("Error reading command from client");
                    }

                    /*
                     * If the client sends the keyword 'modify' and its root,
                     * pass the message to the modify_supplies function,
                     * skipping the keyword and the space character after `modify_`.
                     */
                    if (connected_clients[i].role == 1 && strcmp(command, "modify") == 0)
                    {
                        modify_supplies(buffer + strlen(command) + 1);

                        /* Log json modification */
                        get_client_address(client_socket, client_address, sizeof(client_address));
                        printf("[SERVER] Updated of supplies from client %s\n", client_address);
                        sprintf(log_buffer, "Updated of supplies from client %s", client_address);
                        log_activity(log_buffer);
                    }

                    /* If the clients sends the keyword 'state', send the state summary */
                    if (strcmp(command, "state") == 0)
                    {
                        send_state_summary(client_socket);

                        /* Log activity of get state */
                        get_client_address(client_socket, client_address, sizeof(client_address));
                        printf("[SERVER] Request of state from client %s\n", client_address);
                        sprintf(log_buffer, "Request of state from client %s", client_address);
                        log_activity(log_buffer);
                    }

                    memset(buffer, 0, MAX_BUFFER);
                    memset(command, 0, MAX_BUFFER);
                }
            }
        }

        /* If something happened on the alert pipe, it's an incoming alert. */
        if (FD_ISSET(alert_pipe_fd, &read_fds))
        {
            handle_alerts(alert_pipe_fd);
        }

        /* If something happened on the emergency pipe, it's an incoming emergency. */
        if (FD_ISSET(emergency_pipe_fd, &read_fds))
        {
            handle_emergencies(emergency_pipe_fd);
        }
    }

    // Close the modules sockets and pipes
    close(alert_pipe_fd);
    close(emergency_pipe_fd);
    unlink(ALERT_PIPE_PATH);
    unlink(EMERGENCY_PIPE_PATH);

    return EXIT_SUCCESS;
}

int setup_server(int PORT, int backlog)
{
    int server_socket = 0;
    sockaddr_in server_address;
    int opt = 1;

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons((uint16_t)PORT);

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "[SERVER] Socket creation failed");
    check(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)), "[SERVER] Setsockopt failed");
    check(bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)), "[SERVER] Bind failed");
    check(listen(server_socket, backlog), "[SERVER] Listen failed");

    printf("[SERVER] Listening on %s:%d \n", inet_ntoa(server_address.sin_addr), htons(server_address.sin_port));

    /* CONFIG ALERTS IN A DIFFERENT PROCESS VIA FIFO */
    mkfifo(ALERT_PIPE_PATH, FIFO_PERMISSION);
    alert_pipe_fd = open(ALERT_PIPE_PATH, O_RDONLY | O_NONBLOCK | __O_CLOEXEC);
    pid_t alert_pid = 0;
    check((alert_pid = fork()), "[ERROR] Failed to fork for alerts");
    if (alert_pid == 0)
    {
        start_alerts();
        exit(EXIT_SUCCESS);
    }

    /* CONFIG EMERGENCIES IN A DIFFERENT PROCESS VIA FIFO */
    mkfifo(EMERGENCY_PIPE_PATH, FIFO_PERMISSION);
    emergency_pipe_fd = open(EMERGENCY_PIPE_PATH, O_RDONLY | O_NONBLOCK | __O_CLOEXEC);
    pid_t emergency_pid = 0;
    check((emergency_pid = fork()), "[ERROR] Failed to fork for emergencies");
    if (emergency_pid == 0)
    {
        start_emergencies();
        exit(EXIT_SUCCESS);
    }

    return server_socket;
}

int accept_new_connection(int server_socket)
{
    int add_size = sizeof(sockaddr_in);
    int client_socket = 0;
    int client_role = 0;
    char log_buffer[MAX_LOG_BUFFER];
    char client_address[MAX_BUFFER];
    sockaddr_in client_addr;

    check(client_socket = accept(server_socket, (sockaddr*)&client_addr, (socklen_t*)&add_size), "Accept failed");

    /* Auth client, if the credentials are wrong disconnect him */
    client_role = auth_client(client_socket);
    if (client_role == -1)
    {
        close(client_socket);
        printf("[SERVER] Client could not be authenticated\n");
        return -1;
    }

    connected_clients[num_connected_clients].socket = client_socket;
    connected_clients[num_connected_clients].role = client_role;
    num_connected_clients++;

    printf("[SERVER] New connection at %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    get_client_address(client_socket, client_address, sizeof(client_address));
    sprintf(log_buffer, "Connection from new client %s", client_address);
    log_activity(log_buffer);
    memset(log_buffer, 0, MAX_BUFFER);
    return client_socket;
}

int check(int exp, const char* msg)
{
    if (exp == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}

int auth_client(int client_socket)
{
    ssize_t valread = 0;
    char buffer[MAX_BUFFER];

    /* Receive credentials from the client and auth them. */
    memset(buffer, 0, MAX_BUFFER);
    if ((valread = read(client_socket, buffer, MAX_BUFFER)) <= 0)
    {
        printf("[SERVER] Error reading credentials from client\n");
        close(client_socket);
    }

    buffer[valread] = '\0';
    char username[MAX_BUFFER];
    char password[MAX_BUFFER];
    sscanf(buffer, "%99s %99s", username, password);

    if (strcmp(username, "ubuntu") == 0 && strcmp(password, "ubuntu") == 0)
    {
        send(client_socket, "Authenticated", strlen("Authenticated"), 0);
        return 1;
    }

    if (strcmp(username, "hostname") == 0 && strcmp(password, "hostname") == 0)
    {
        send(client_socket, "Authenticated", strlen("Authenticated"), 0);
        return 0;
    }

    send(client_socket, "Denied", strlen("Denied"), 0);
    close(client_socket);
    return -1;
}

void log_activity(const char* activity)
{
    time_t current_time;
    struct tm* local_time;
    char timestamp[MAX_LOG_BUFFER];

    current_time = time(NULL);
    check(current_time != -1, "Error getting current time");

    local_time = localtime(&current_time);
    check(local_time != NULL, "Error converting time to local time");

    strftime(timestamp, sizeof(timestamp), "%a %b %d %T %Y", local_time);

    FILE* log_file = fopen("server.log", "ae");
    check(log_file != NULL, "Error opening log file");

    fprintf(log_file, "%s, %s\n", timestamp, activity);

    fclose(log_file);
}

void get_client_address(int client_socket, char* client_address, size_t buffer_size)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    /* Retrieve the address information associated with the client socket */
    if (getpeername(client_socket, (struct sockaddr*)&addr, &addr_len) == 0)
    {
        /* Format the address into the provided buffer */
        snprintf(client_address, buffer_size, "%s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }
    else
    {
        perror("Error getting client address");
        exit(EXIT_FAILURE);
    }
}

void handle_alerts(int alert_pipe_fd)
{
    char buffer[MAX_BUFFER];
    ssize_t bytes_read = 0;

    while ((bytes_read = read(alert_pipe_fd, buffer, MAX_BUFFER)) > 0)
    {
        // Remove newline character from the received message
        buffer[bytes_read - 1] = '\0';

        modify_alerts(buffer);

        printf("[ALERT] %s\n", buffer);
        log_activity(buffer);

        // Notify all connected clients about the alert
        strcat(buffer, "\n");
#pragma unroll
        for (int i = 0; i < num_connected_clients; ++i)
        {
            int client_socket = connected_clients[i].socket;
            if (client_socket > 0)
            {
                send(client_socket, buffer, strlen(buffer), 0);
            }
        }

        memset(buffer, 0, MAX_BUFFER);
    }

    if (bytes_read == 0)
    {
        printf("[ALERT] Alert pipe closed\n");
    }
}

void handle_emergencies(int emergency_pipe_fd)
{
    char buffer[MAX_BUFFER];
    ssize_t bytes_read = 0;

    bytes_read = read(emergency_pipe_fd, buffer, MAX_BUFFER);
    if (bytes_read <= 0)
    {
        return;
    }

    // Remove newline character from the received message
    buffer[bytes_read - 1] = '\0';

    modify_emergencies(buffer);

    printf("[EMERGENCY] %s\n", buffer);
    log_activity(buffer);

    // Notify all clients about the emergency
    strcat(buffer, "\n");
#pragma unroll
    for (int i = 0; i < num_connected_clients; ++i)
    {
        int client_socket = connected_clients[i].socket;
        if (client_socket > 0)
        {
            send(client_socket, buffer, strlen(buffer), 0);
        }
    }

    disconnect_all_clients();
}

void disconnect_client(int client_socket, int client_index)
{
    char client_address[INET_ADDRSTRLEN];
    char log_buffer[MAX_BUFFER];

    get_client_address(client_socket, client_address, sizeof(client_address));
    printf("[SERVER] Client disconnected at %s\n", client_address);
    sprintf(log_buffer, "Connection close from client %s", client_address);
    log_activity(log_buffer);

    close(client_socket);

/* Shift elements in the connected_clients array to fill the gap */
#pragma unroll
    for (int j = client_index; j < num_connected_clients - 1; j++)
    {
        connected_clients[j] = connected_clients[j + 1];
    }

    /* Clear the last element (previously occupied by the disconnected client) */
    connected_clients[num_connected_clients - 1].socket = 0;
    connected_clients[num_connected_clients - 1].role = 0;
    num_connected_clients--;
}

void disconnect_all_clients()
{
    char client_address[INET_ADDRSTRLEN];
    char log_buffer[MAX_BUFFER];

// Disconnect all clients
#pragma unroll
    for (int i = 0; i < num_connected_clients; ++i)
    {
        int client_socket = connected_clients[i].socket;
        if (client_socket > 0)
        {
            // Get client address
            get_client_address(client_socket, client_address, sizeof(client_address));
            printf("[SERVER] Client disconnected at %s\n", client_address);
            sprintf(log_buffer, "Connection close from client %s", client_address);
            log_activity(log_buffer);

            // Close client socket
            close(client_socket);

            // Mark the client as disconnected
            connected_clients[i].socket = 0;
            connected_clients[i].role = 0;
        }
    }

    num_connected_clients = 0;
}

void setup_shared_memory()
{
    int fds = 0;
    void* ptr = NULL;

    check((fds = shm_open(SMO_NAME, O_CREAT | O_RDWR, FIFO_PERMISSION)), "[ERROR] shm_open failed");
    check(ftruncate(fds, SMO_SIZE), "[ERROR] ftruncate failed");

    ptr = mmap(0, SMO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fds, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    create_state_summary();
}
