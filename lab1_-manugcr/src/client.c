#include "client.h"

int main(int argc, char const* argv[])
{
    int client_socket = 0;
    char buffer[MAX_BUFFER];
    struct sockaddr_in server_address;

    /**
     * Step 0
     *  - Ask for username and password
     *  - Ask for IP and PORT
     */
    if (argc != 3)
    {
        printf("Usage: %s <username> <password>\n", argv[0]);
        // printf("Usage: %s <username> <password> <IP> <PORT>\n", argv[0])
        exit(EXIT_FAILURE);
    }
    const char* username = argv[1];
    const char* password = argv[2];
    // const char* ip = argv[3];
    // int port = atoi(argv[4]);

    /**
     * Step 1
     *  - Create a TCP socket.
     */
    client_socket = check(socket(AF_INET, SOCK_STREAM, 0), "[CLIENT] Socket creation failed");

    /* Set the socket to non-blocking to send and get messages without blocking the terminal. */
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    /* Set the client as TCP */
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    /* Convert IPv4 and IPv6 addresses from text to binary form */
    check(inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr), "[CLIENT] Invalid address/Address not supported");

    /**
     * Step 2
     * - Connect to the server to get authenticated
     */
    check(connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)),
          "[CLIENT] Connection failed");

    /**
     * Step 3
     * - Auth credentials with server
     */
    int snprintf_result = snprintf(buffer, MAX_BUFFER, "%s %s", username, password);
    if (snprintf_result < 0)
    {
        perror("[CLIENT] Snprintf failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    send(client_socket, buffer, MAX_BUFFER, 0);
    memset(buffer, 0, MAX_BUFFER);

    if (recv(client_socket, buffer, MAX_BUFFER, 0) < 0)
    {
        perror("[CLIENT] Recieve failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    if (strcmp(buffer, "Denied") == 0)
    {
        printf("[SERVER]\nAuthentication failed\n");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("[SERVER]\nAuthentication succeed.\n");

    /**
     * Step 4
     * - Connect to the server
     */
    #pragma unroll
    while (TRUE)
    {
        char input[MAX_BUFFER];

        /* Send messages to the server. */
        if (fgets(input, MAX_BUFFER, stdin) != NULL)
        {
            send(client_socket, input, strlen(input), 0);
        }

        /* Receive messages from the server */
        ssize_t valread = recv(client_socket, buffer, MAX_BUFFER, MSG_DONTWAIT);
        if (valread < 0)
        {
            continue;
        }

        if (valread == 0)
        {
            break;
        }

        buffer[valread] = '\0';
        printf("[SERVER]\n%s\n", buffer);

        memset(buffer, 0, MAX_BUFFER);
    }

    close(client_socket);

    return EXIT_SUCCESS;
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
