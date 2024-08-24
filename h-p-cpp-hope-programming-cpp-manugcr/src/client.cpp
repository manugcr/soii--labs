#include "client.hpp"

bool receive_compressed_file(int sockfd, char buffer[MAX])
{
    std::ofstream zipFile(ZIP_FILE, std::ios::binary);
    if (!zipFile.is_open())
    {
        std::cerr << "[ERROR] Error opening zip file for writing" << strerror(errno) << std::endl;
        return false;
    }

    ssize_t bytesRead;
    size_t totalBytesReceived = 0;

    while ((bytesRead = recv(sockfd, buffer, MAX, 0)) > 0)
    {
        if (strncmp(&buffer[bytesRead - 3], "EOF", 3) == 0)
        {
            zipFile.write(buffer, bytesRead - 3);
            totalBytesReceived += bytesRead;
            break;
        }
        if (!zipFile.write(buffer, bytesRead))
        {
            std::cerr << "[ERROR] Error writing to zip file" << std::endl;
            zipFile.close();
            close(sockfd);
            return false;
        }
        totalBytesReceived += bytesRead;
    }

    std::cout << "[SERVER] Received " << totalBytesReceived << " bytes" << std::endl;
    zipFile.close();

    return decompress_file(ZIP_FILE, RECV_IMG);
}

bool decompress_file(const std::string& input_path_zip, const std::string& output_path_png)
{
    gzFile zipFile = gzopen(input_path_zip.c_str(), "rb");
    if (zipFile == NULL)
    {
        std::cerr << "[ERROR] Error opening zip file for reading" << std::endl;
        return false;
    }

    std::ofstream pngFile(output_path_png, std::ios::binary);
    if (!pngFile.is_open())
    {
        std::cerr << "[ERROR] Error opening file for writing" << std::endl;
        gzclose(zipFile);
        return false;
    }

    char buffer[MAX];
    int bytesRead = 0;
    while ((bytesRead = gzread(zipFile, buffer, MAX)) > 0)
    {
        if (!pngFile.write(buffer, bytesRead))
        {
            std::cerr << "[ERROR] Error writing to file" << std::endl;
            pngFile.close();
            gzclose(zipFile);
            return false;
        }
    }
    gzclose(zipFile);
    pngFile.close();

    std::cout << "[SERVER] Decompressed file to " << output_path_png << std::endl;

    return TRUE;
}

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    char input[MAX];
    char buffer[MAX];

    /* STEP 0 - Ask for IP and PORT */
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <PORT> <IP>" << std::endl;
        exit(EXIT_FAILURE);
    }
    PORT = std::stoi(argv[1]);
    ADDR = argv[2];

    /* STEP 1 - Create a socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "[ERROR] Socket creation failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Set the socket to non-blocking to send and get messages without blocking the terminal. */
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    /* Set server ip and port */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ADDR, &servaddr.sin_addr) <= 0)
    {
        std::cerr << "[ERROR] Invalid address/Address not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    /* STEP 2 - Connect to the server */
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        std::cerr << "[ERROR] Connection with the server failed." << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "[CLIENT] Connected at " << ADDR << ":" << PORT << std::endl;
    }

    /* STEP 3 - Communicate with the server */
    while (TRUE)
    {
        ssize_t valread = recv(sockfd, buffer, MAX, MSG_DONTWAIT);

        if (fgets(input, MAX, stdin) != NULL)
        {
            // Replace newline with null terminator
            size_t len = strlen(input);
            input[len - 1] = '\0';
            send(sockfd, input, strlen(input), 0);

            if (strcmp(input, "get_image") == 0)
            {
                char buffer[MAX];
                if (receive_compressed_file(sockfd, buffer))
                {
                    std::cout << "[SERVER] Image received." << std::endl;
                    ;
                }
                else
                {
                    std::cout << "[ERROR] Error receiving image." << std::endl;
                    ;
                }
            }
        }

        if (valread <= 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                // No data available, continue as non-blocking
                continue;
            }
            else
            {
                std::cerr << "[ERROR] Connection lost." << std::endl;
                break;
            }
        }
        else
        {
            // Check if received message is the disconnect message
            if (strncmp(buffer, "exit", strlen("exit")) == 0)
            {
                std::cout << "[EMERGENCY] Emergency detected, all clients disconnected." << std::endl;
                break;
            }
        }

        buffer[valread] = '\0';
        std::cout << "[SERVER] " << buffer << std::endl;
        memset(buffer, 0, MAX);
    }

    close(sockfd);
    return 0;
}
