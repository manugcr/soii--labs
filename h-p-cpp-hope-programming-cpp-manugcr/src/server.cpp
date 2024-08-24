#include "server.hpp"
#include "alerts.hpp"
#include "cannyEdgeFilter.hpp"
#include "emergency.hpp"
#include "supplies.hpp"

void get_time_stamp(char* timestamp)
{
    std::time_t now = std::time(nullptr);
    std::strftime(timestamp, 80, "%a %b %d %T %Y", std::localtime(&now));
}

void get_port(const struct sockaddr_storage& clientAddr, int& port)
{
    if (clientAddr.ss_family == AF_INET)
    {
        struct sockaddr_in* s = (struct sockaddr_in*)&clientAddr;
        port = ntohs(s->sin_port);
    }
    else
    {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&clientAddr;
        port = ntohs(s->sin6_port);
    }
}

void get_ip(const struct sockaddr_storage& clientAddr, char* ipstr)
{
    if (clientAddr.ss_family == AF_INET)
    {
        struct sockaddr_in* s = (struct sockaddr_in*)&clientAddr;
        inet_ntop(AF_INET, &s->sin_addr, ipstr, INET_ADDRSTRLEN);
    }
    else
    {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&clientAddr;
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, INET6_ADDRSTRLEN);
    }
}

void get_refuge_state(int client_sock, rocksdb::DB* db, rocksdb::Status status)
{
    std::string refugeString = show_state_summary(db, status);
    send(client_sock, refugeString.c_str(), refugeString.size(), 0);
    return;
}

void set_supplies(int client_sock, rocksdb::DB* db, rocksdb::Status status)
{
    char buffer[MAX_BUFFER];
    ssize_t n;

    send(client_sock, "Enter the new 'key,value' supply:", 32, 0);
    n = recv(client_sock, buffer, MAX_BUFFER, 0);
    if (n <= 0)
    {
        std::cerr << "Error reading from client" << std::endl;
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    std::string supplies = std::string(buffer);
    std::string supply_key = supplies.substr(0, supplies.find(","));
    std::string new_value = supplies.substr(supplies.find(",") + 1);
    modify_supplies(db, supply_key, new_value);
    send(client_sock, "Supplies updated.", 17, 0);
    get_refuge_state(client_sock, db, status);

    return;
}

void log_event(const struct sockaddr_storage& clientAddr, const std::string& eventType,
               const std::string& eventDescription = "")
{
    std::ofstream logfile("../tmp/server_log.log", std::ios_base::app);
    if (!logfile.is_open())
    {
        std::cerr << "Error opening log file." << std::endl;
        return;
    }

    char timestamp[80];
    get_time_stamp(timestamp);

    // Extract port
    int port;
    get_port(clientAddr, port);

    char ipstr[INET6_ADDRSTRLEN];
    get_ip(clientAddr, ipstr);

    logfile << timestamp << ", " << eventType << " from client (" << ipstr << ":" << port << ")";
    if (!eventDescription.empty())
    {
        logfile << " - " << eventDescription;
    }
    logfile << std::endl;

    logfile.close();
}

void log_alert(bool isAlert, const std::string& message)
{
    std::ofstream logfile("../tmp/server_log.log", std::ios_base::app);
    if (!logfile.is_open())
    {
        std::cerr << "[ERROR] Error opening log file." << std::endl;
        return;
    }

    char timestamp[80];
    get_time_stamp(timestamp);

    logfile << timestamp << ", ";
    if (isAlert)
    {
        logfile << "Alert of possible infection in " << message << " entry." << std::endl;
    }
    else
    {
        logfile << "Emergency notification. " << message;
    }

    logfile.close();
}

bool check_credentials(const std::string& username, const std::string& password)
{
    user = username == "ubuntu" && password == "ubuntu";
    return user;
}

void http_functions(rocksdb::DB* db, rocksdb::Status status)
{
    httplib::Server svr;
    create_state_summary(db);

    svr.Get("/root", [](const httplib::Request& req, httplib::Response& res) {
        std::string html = setup_html("../html/root.html");
        res.set_content(html, "text/html");
    });

    svr.Get("/supplies", [db, &status](const httplib::Request& req, httplib::Response& res) {
        std::string suppliesString = get_supplies(db, status);
        res.set_content(suppliesString, "text/plain");
    });

    svr.Get("/alerts", [db, &status](const httplib::Request& req, httplib::Response& res) {
        std::string alertsString = get_alerts(db, status);
        res.set_content(alertsString, "text/plain");
    });

    svr.Get("/emergencies", [db, &status](const httplib::Request& req, httplib::Response& res) {
        std::string emergenciesString = get_emergencies(db, status);
        res.set_content(emergenciesString, "text/plain");
    });

    svr.Get("/modify_supply", [](const httplib::Request& req, httplib::Response& res) {
        std::string html = setup_html("../html/login.html");
        res.set_content(html, "text/html");
    });

    svr.Get("/modify_form", [](const httplib::Request& req, httplib::Response& res) {
        std::string html = setup_html("../html/modify.html");
        res.set_content(html, "text/html");
    });

    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        auto username = req.get_param_value("username");
        auto password = req.get_param_value("password");
        if (check_credentials(username, password))
        {
            res.set_redirect("/modify_form");
        }
        else
        {
            res.set_content("Invalid username or password", "text/plain");
            res.status = 401;
        }
    });

    // Manage the POST request to modify the supplies.
    svr.Post("/modify", [db](const httplib::Request& req, httplib::Response& res) {
        auto supply_key = req.get_param_value("supply_key");
        auto new_value = req.get_param_value("new_value");
        if (user)
        {
            modify_supplies(db, supply_key, new_value);
            res.set_redirect("/modify_form");
        }
        else
        {
            res.set_content("You are not authorized to modify the supplies.", "text/plain");
            res.status = 401;
        }
    });

    svr.listen("localhost", 8082);
}

bool client_login_with_bool_return(int client_sock, bool& ubuntu)
{
    char buffer[MAX_BUFFER];
    ssize_t n;

    send(client_sock, "Enter username: ", 17, 0);
    n = recv(client_sock, buffer, MAX_BUFFER, 0);
    if (n <= 0)
    {
        std::cerr << "[ERROR] Error reading from client." << std::endl;
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    if (std::string(buffer) == "ubuntu")
    {
        send(client_sock, "Enter password: ", 17, 0);
        n = recv(client_sock, buffer, MAX_BUFFER, 0);
        if (n <= 0)
        {
            std::cerr << "[ERROR] Error reading from client." << std::endl;
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        if (std::string(buffer) == "ubuntu")
        {
            std::cout << "[SERVER] Login successful." << std::endl;
            send(client_sock, "Login successful.", 18, 0);
            ubuntu = true;
        }
        else
        {
            std::cout << "[SERVER] Login failed." << std::endl;
        }
    }

    return ubuntu;
}

void handle_alerts(int alert_fifo_fd, rocksdb::DB* db)
{
    char buffer[MAX_BUFFER];
    ssize_t n;
    n = read(alert_fifo_fd, buffer, MAX_BUFFER);
    if (n <= 0)
    {
        std::cerr << "[ERROR] Error reading from alert pipe" << std::endl;
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';

    // Tokenize the message using strtok
    char* entry = strtok(buffer, ",");
    char* temperature = strtok(NULL, ",");

    // Sending alert message to all connected clients
    std::string alert_message = "Alert of possible infection in ";
    alert_message += entry;
    alert_message += " entry.";

    for (const auto& client : connected_clients)
    {
        send(client.socket, alert_message.c_str(), alert_message.size(), 0);
    }

    std::cout << "[ALERT] Alert of possible infection in " << entry << " entry, " << temperature;
    log_alert(true, entry);
    modify_alerts(db, entry);
}

void handle_emergency(int emgs_fifo_fd, rocksdb::DB* db)
{
    std::time_t now = std::time(nullptr);
    char timestamp[80];
    std::strftime(timestamp, sizeof(timestamp), "%a %b %d %T %Y", std::localtime(&now));

    char buffer[MAX_BUFFER];
    ssize_t n;
    n = read(emgs_fifo_fd, buffer, MAX_BUFFER);
    if (n <= 0)
    {
        std::cerr << "[ERROR] Error reading from emergency pipe" << std::endl;
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';

    std::cout << "[EMERGENCY] " << buffer;
    log_alert(false, buffer);
    modify_emergencies(db, timestamp, buffer);
    disconnect_all_clients();
}

void handle_client(int client_socket, rocksdb::DB* db, rocksdb::Status status)
{
    char buffer[MAX_BUFFER];
    ssize_t bytes_received;
    bool ubuntu = false;

    // Retrieve client information for logging
    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);
    if (getpeername(client_socket, (struct sockaddr*)&client_addr, &addr_len) == -1)
    {
        std::cerr << "[ERROR] Error getting client information." << std::endl;
        close(client_socket);
        return;
    }

    // Extract IP address and port
    char ipstr[INET_ADDRSTRLEN];
    int port;
    if (client_addr.ss_family == AF_INET)
    {
        struct sockaddr_in* s = (struct sockaddr_in*)&client_addr;
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
        port = ntohs(s->sin_port);
    }
    else
    {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&client_addr;
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof(ipstr));
        port = ntohs(s->sin6_port);
    }

    // Add the client to the list of connected clients
    connected_clients.push_back({client_socket});

    // Log the connection
    log_event(client_addr, "Connection from new client");
    std::cout << "[SERVER] Client connected at port: " << port << std::endl;

    while (true)
    {
        bytes_received = recv(client_socket, buffer, MAX_BUFFER - 1, 0);
        if (bytes_received <= 0)
        {
            break;
        }
        if (std::string(buffer) == "exit")
        {
            break;
        }
        buffer[bytes_received] = '\0';

        if (std::string(buffer) == "login")
        {
            std::cout << "[SERVER] Client " << port << " is attempting to login." << std::endl;
            ubuntu = client_login_with_bool_return(client_socket, ubuntu);
        }
        else if (std::string(buffer) == "set_supplies")
        {
            if (ubuntu)
            {
                set_supplies(client_socket, db, status);
                log_event(client_addr, "Update of supplies from client",
                          ubuntu ? "Authorized" : "Denied invalid hostname");
            }
            else
            {
                std::cout << "[SERVER] Client " << port << " is not authorized to modify the supplies." << std::endl;
                send(client_socket, "You are not authorized to modify the supplies.", 40, 0);
                log_event(client_addr, "Update of supplies from client",
                          ubuntu ? "Authorized" : "Denied invalid hostname");
            }
        }
        else if (std::string(buffer) == "get_state")
        {
            std::cout << "[SERVER] Client " << port << " requested the state." << std::endl;
            get_refuge_state(client_socket, db, status);
            log_event(client_addr, "Request of state from client");
        }
        else if (std::string(buffer) == "get_image")
        {
            std::cout << "[SERVER] Client " << port << " requested the image." << std::endl;
            log_event(client_addr, "Request of image from client");
            send_zip_file(client_socket);
        }
        else if (std::string(buffer) == "help" || std::string(buffer) == "h")
        {
            std::cout << "[SERVER] Client " << port << " requested help." << std::endl;
            const char* message = "Commands available:\n"
                                  " -> help          - Show this help message.\n"
                                  " -> login         - login as root to modify supplies.\n"
                                  " -> exit          - Disconnect from the server.\n"
                                  " -> get_image     - Get satellite image from canny edge.\n"
                                  " -> get_state     - Get summary state of the refuge.\n"
                                  " -> set_supplies  - Set supplies for the refuge.";
            send(client_socket, message, strlen(message), 0);
        }
        else
        {
            std::cout << "[SERVER] Invalid command " << buffer << std::endl;
            send(client_socket, "Invalid command.", 16, 0);
        }

        /* Clear the buffer. */
        memset(buffer, 0, MAX_BUFFER);
    }

    // Remove the client from the list of connected clients
    connected_clients.erase(std::remove_if(connected_clients.begin(), connected_clients.end(),
                                           [client_socket](const Client& c) { return c.socket == client_socket; }),
                            connected_clients.end());

    std::cout << "[SERVER] Client " << port << " disconnected." << std::endl;
    log_event(client_addr, "Connection close from client");

    close(client_socket);
}

void disconnect_all_clients()
{
    // Message to be sent to clients before disconnecting
    std::string disconnect_message = "exit";

    for (const auto& client : connected_clients)
    {
        // Send the disconnect message to each client
        send(client.socket, disconnect_message.c_str(), disconnect_message.length(), 0);

        // Print the client that is being disconnected
        std::cout << "[SERVER] Disconnecting client socket " << client.socket << std::endl;

        // Close the client's socket
        close(client.socket);
    }

    // Clear the list of connected clients
    connected_clients.clear();
}

std::string setup_html(const std::string& filename)
{
    std::ifstream file(filename);
    if (file)
    {
        std::string html_content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
        return html_content;
    }
    else
    {
        std::cerr << "[ERROR] Cannot open file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
}

void setup_server(int& server_socket_ipv4, int& server_socket_ipv6, sockaddr_in& server_addr_ipv4,
                  sockaddr_in6& server_addr_ipv6)
{
    /* Create an IPv4 socket */
    if ((server_socket_ipv4 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "[ERROR] Cannot create IPv4 socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    server_addr_ipv4.sin_family = AF_INET;
    server_addr_ipv4.sin_port = htons(8080); // change port to client input
    server_addr_ipv4.sin_addr.s_addr = INADDR_ANY;

    /* Bind the IPv4 socket to an IP and port */
    if (bind(server_socket_ipv4, (struct sockaddr*)&server_addr_ipv4, sizeof(server_addr_ipv4)) < 0)
    {
        std::cerr << "[ERROR] IPv4 bind failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Start listening for connections on the IPv4 socket */
    if (listen(server_socket_ipv4, 10) < 0)
    {
        std::cerr << "[ERROR] IPv4 listen failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Create an IPv6 socket */
    if ((server_socket_ipv6 = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "[ERROR] Cannot create IPv6 socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    server_addr_ipv6.sin6_family = AF_INET6;
    server_addr_ipv6.sin6_port = htons(8081);
    server_addr_ipv6.sin6_addr = in6addr_any;

    /* Bind the IPv6 socket to an IP and port */
    if (bind(server_socket_ipv6, (struct sockaddr*)&server_addr_ipv6, sizeof(server_addr_ipv6)) < 0)
    {
        std::cerr << "[ERROR] IPv6 bind failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Start listening for connections on the IPv6 socket */
    if (listen(server_socket_ipv6, 10) < 0)
    {
        std::cerr << "[ERROR] IPv6 listen failed." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void setup_rocksdb(rocksdb::DB*& db)
{
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, DB_PATH, &db);
    if (!status.ok())
    {
        std::cerr << "[ERROR] Error opening database: " << status.ToString() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void setup_alerts_fifo()
{
    mkfifo(ALERT_PIPE, 0666);
    alert_fifo_fd = open(ALERT_PIPE, O_RDONLY | O_NONBLOCK);
    pid_t alert_pid = fork();
    if (alert_pid < 0)
    {
        std::cerr << "Error forking alert child process\n";
        exit(EXIT_FAILURE);
    }
    if (alert_pid == 0)
    {
        run_invasion_alerts();
        exit(EXIT_SUCCESS);
    }
}

void setup_emgs_fifo()
{
    mkfifo(EMGS_PIPE, 0666);
    emgs_fifo_fd = open(EMGS_PIPE, O_RDONLY | O_NONBLOCK);
    pid_t emgs_pid = fork();
    if (emgs_pid < 0)
    {
        std::cerr << "Error forking emergency child process\n";
        exit(EXIT_FAILURE);
    }
    if (emgs_pid == 0)
    {
        run_emergencies();
        exit(EXIT_SUCCESS);
    }
}

void start_canny_edge_detection()
{
    std::cout << "[EDGE] Canny edge detection started." << std::endl;

    // Provide the filename of the satellite image
    std::string inputImageFilename = "../canny_imgs/Satellite2.tif";

    // Specify the output filename for the edge-detected image
    std::string outputImageFilename = EDGE_IMAGE;

    // Instantiate EdgeDetection object
    EdgeDetection edgeDetector(50, 150, 1.0);

    // Apply Canny edge detection to the satellite image
    edgeDetector.cannyEdgeDetection(inputImageFilename, outputImageFilename);

    // Indicate that the process is complete
    std::cout << "[EDGE] Canny edge detection applied successfully." << std::endl;

    compress_image();
}

void compress_image()
{
    std::cout << "[EDGE] Creating zip file." << std::endl;

    gzFile zip_file = gzopen(ZIP_FILE, "wb");
    if (zip_file == NULL)
    {
        std::cerr << "[EDGE] Error creating zip file." << std::endl;
        return;
    }

    FILE* image_file = fopen(EDGE_IMAGE, "rb");
    if (image_file == NULL)
    {
        std::cerr << "[EDGE] Error opening image file." << std::endl;
        return;
    }

    char buffer[MAX_BUFFER];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), image_file)) > 0)
    {
        if (gzwrite(zip_file, buffer, bytes) == 0)
        {
            std::cerr << "[EDGE] Error writing to zip file." << std::endl;
            fclose(image_file);
            gzclose(zip_file);
            return;
        }
    }

    fclose(image_file);
    gzclose(zip_file);

    std::cout << "[EDGE] Zip file created succesfully." << std::endl;
}

void send_zip_file(int client_socket)
{
    char buffer[MAX_BUFFER];
    int num;

    FILE* zip_file = fopen(ZIP_FILE, "rb");
    if (zip_file == NULL)
    {
        std::cerr << "[EDGE] Error opening zip file." << std::endl;
        return;
    }

    ssize_t bytes_read;
    size_t bytes_sent = 0;
    while ((bytes_read = fread(buffer, 1, MAX_BUFFER, zip_file)) > 0)
    {
        num = write(client_socket, buffer, bytes_read);
        if (num < 0)
        {
            std::cerr << "[EDGE] Error sending zip file." << std::endl;
            fclose(zip_file);
            return;
        }
        bytes_sent += num;
    }

    if (bytes_read < 0)
    {
        std::cerr << "[EDGE] Error reading zip file." << std::endl;
        fclose(zip_file);
        return;
    }

    std::cout << "[EDGE] Zip file sent successfully. " << bytes_sent << " bytes." << std::endl;

    const char* eof = "EOF";
    num = write(client_socket, static_cast<const void*>(eof), strlen(eof));
    if (num < 0)
    {
        std::cerr << "[EDGE] Error sending EOF." << std::endl;
        return;
    }
}

int main(void)
{

    /* ROCKSDB Setup */
    rocksdb::DB* db;
    rocksdb::Status status;
    setup_rocksdb(db);

    /* HTTPLIB Setup */
    std::thread(http_functions, db, status).detach();

    /* SERVER Setup */
    int server_socket_ipv4, server_socket_ipv6, client_socket;
    struct sockaddr_in server_addr_ipv4;
    struct sockaddr_in6 server_addr_ipv6;
    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_storage);
    setup_server(server_socket_ipv4, server_socket_ipv6, server_addr_ipv4, server_addr_ipv6);
    setup_alerts_fifo();
    setup_emgs_fifo();

    std::cout << "\n==================================================================" << std::endl;
    std::cout << "========================= SERVER RUNNING =========================" << std::endl;
    std::cout << "==================================================================" << std::endl;

    /* Process images */
    start_canny_edge_detection();

    fd_set read_fds;
    int max_fd;
    while (true)
    {
        FD_ZERO(&read_fds);
        FD_SET(server_socket_ipv4, &read_fds);
        FD_SET(server_socket_ipv6, &read_fds);
        FD_SET(alert_fifo_fd, &read_fds);
        FD_SET(emgs_fifo_fd, &read_fds);
        max_fd = std::max({server_socket_ipv4, server_socket_ipv6, alert_fifo_fd, emgs_fifo_fd}) + 1;

        // Wait for activity on any of the sockets
        if (select(max_fd, &read_fds, NULL, NULL, NULL) < 0)
        {
            std::cerr << "[ERROR] Select failed." << std::endl;
            return 1;
        }

        // Accept a new client connection on the IPv4 socket
        if (FD_ISSET(server_socket_ipv4, &read_fds))
        {
            if ((client_socket = accept(server_socket_ipv4, (struct sockaddr*)&client_addr, &addr_len)) < 0)
            {
                std::cerr << "[ERROR] IPv4 accept failed." << std::endl;
                return 1;
            }

            // Create a new thread to handle the client connection
            std::thread(handle_client, client_socket, db, status).detach();
        }

        // Accept a new client connection on the IPv6 socket
        if (FD_ISSET(server_socket_ipv6, &read_fds))
        {
            if ((client_socket = accept(server_socket_ipv6, (struct sockaddr*)&client_addr, &addr_len)) < 0)
            {
                std::cerr << "[ERROR] IPv6 accept failed." << std::endl;
                return 1;
            }

            // Create a new thread to handle the client connection
            std::thread(handle_client, client_socket, db, status).detach();
        }

        // Check for alerts from the alert fork
        if (FD_ISSET(alert_fifo_fd, &read_fds))
        {
            handle_alerts(alert_fifo_fd, db);
        }

        // Check for emergencies from the emergency fork
        if (FD_ISSET(emgs_fifo_fd, &read_fds))
        {
            handle_emergency(emgs_fifo_fd, db);
        }
    }

    /* Clean up resources */
    close(server_socket_ipv4);
    close(server_socket_ipv6);
    close(alert_fifo_fd);
    close(emgs_fifo_fd);
    unlink(ALERT_PIPE);
    unlink(EMGS_PIPE);
    delete db;

    return 0;
}
