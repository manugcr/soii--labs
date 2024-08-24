#ifndef _SERVER_HPP
#define _SERVER_HPP

#include <algorithm>
#include <cstdlib> // For exit
#include <cstring> // For strtok, strcpy
#include <ctime>
#include <filesystem>
#include <fstream>
#include <httplib.h>
#include <iostream>
#include <rocksdb/db.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <zlib.h>

#define IP_TCP "127.0.0.1"
#define IP_UDP "::1"

#define DB_PATH "../tmp/refuge_db"
#define ALERT_PIPE "../tmp/alert_pipe"
#define EMGS_PIPE "../tmp/emgs_pipe"
#define LOG_FILE "../tmp/server.log"
#define ZIP_FILE "../tmp/edge_file.zip"
#define EDGE_IMAGE "../tmp/edge_file.png"
#define MAX_MESSAGE_LENGTH 1024
#define BACKLOG 10
#define MAX_BUFFER 4096
#define MAX_CLIENTS 100

/**
 * @brief Represents a connected client.
 */
struct Client
{
    int socket; /**< The socket descriptor of the client. */
};

std::vector<Client> connected_clients; // Vector to hold connected clients

bool user = false; // false = normal, true = ubuntu
int alert_fifo_fd;
int emgs_fifo_fd;
pid_t alert_pid = -1;
pid_t emgs_pid = -1;

/**
 * @brief Start the Canny edge detection process.
 *
 * Uses the cannyEdgeFilter lib to apply Canny edge detection to a satellite image.
 *
 * @return void
 */
void start_canny_edge_detection();

/**
 * @brief Send the edge-detected image to the client.
 *
 * Send the edge-detected image to the client using the client socket.
 * The image is sent byte by byte to the client.
 *
 * @param client_socket The client socket.
 * @return void
 */
void send_zip_file(int client_socket);

/**
 * @brief Compress the edge-detected image.
 *
 * Compress the edge-detected image using zlib.
 *
 * @return void
 */
void compress_image();

/**
 * @brief Handle the client connection.
 *
 * @param client_socket The client socket.
 * @param db The database.
 * @param status The status of the database.
 * @return void
 */
void handle_client(int client_socket, rocksdb::DB* db, rocksdb::Status status);

/**
 * @brief Handle the alerts from the alert FIFO.
 *
 * @param alert_fifo_fd The alert FIFO file descriptor.
 * @param db The database.
 * @return void
 */
void handle_alerts(int alert_fifo_fd, rocksdb::DB* db);

/**
 * @brief Handle the emergencies from the emergency FIFO.
 *
 * @param emgs_fifo_fd The emergency FIFO file descriptor.
 * @param db The database.
 * @return void
 */
void handle_emergencies(int emgs_fifo_fd, rocksdb::DB* db);

/**
 * @brief Update the supplies in the database.
 *
 * @param client_sock The client socket.
 * @param db The database.
 * @param status The status of the database.
 * @return void
 */
void set_supplies(int client_sock, rocksdb::DB* db, rocksdb::Status status);

/**
 * @brief Handle the client login.
 *
 * @param client_sock The client socket.
 * @param ubuntu The boolean to store the login status.
 * @return bool The login status.
 */
bool clientLoginWithBoolReturn(int client_sock, bool& ubuntu);

/**
 * @brief Show the state summary to the client.
 *
 * @param client_sock The client socket.
 * @param db The database.
 * @param status The status of the database.
 * @return void
 */
void get_refuge_state(int client_socket, rocksdb::DB* db, rocksdb::Status status);

/**
 * @brief Handle the HTTP functions.
 *
 * @param db The database.
 * @param status The status of the database.
 * @return void
 */
void http_functions(rocksdb::DB* db, rocksdb::Status status);

/**
 * @brief Read the contents of an HTML file.
 *
 * @param filename The name of the file to read.
 * @return std::string The contents of the file.
 * @note The file must be in the html directory.
 */
std::string setup_html(const std::string& filename);

/**
 * @brief Check the credentials of the client.
 *
 * @param username The username of the client.
 * @param password The password of the client.
 * @return bool True if the credentials are correct, false otherwise.
 */
bool check_credentials(const std::string& username, const std::string& password);

/**
 * @brief Log an event to the server log file.
 *
 * @param clientAddr The client address.
 * @param eventType The type of event.
 * @param eventDescription The description of the event.
 * @return void
 */
void log_event(const struct sockaddr_storage& clientAddr, const std::string& eventType,
               const std::string& eventDescription);

/**
 * @brief Log an alert to the server log file.
 *
 * @param isAlert True if the log is an alert, false if it is an emergency.
 * @param message The message to log.
 * @return void
 */
void log_alert(bool isAlert, const std::string& entry);

/**
 * @brief Create the alerts FIFO.
 *
 * @return void
 */
void setup_alerts_fifo();

/**
 * @brief Create the emergencies FIFO.
 *
 * @return void
 */
void setup_emgs_fifo();

/**
 * @brief Get the current time stamp in the format "Day Month Date Time Year".
 *
 * @param timestamp The buffer to store the time stamp.
 * @return void
 */
void get_time_stamp(char* timestamp);

/**
 * @brief Get the port number from the client address.
 *
 * @param clientAddr The client address.
 * @param port The port number.
 * @return void
 */
void get_port(const struct sockaddr_storage& clientAddr, int& port);

/**
 * @brief Get the IP address from the client address.
 *
 * @param clientAddr The client address.
 * @param ipstr The buffer to store the IP address.
 * @return void
 */
void get_ip(const struct sockaddr_storage& clientAddr, char* ipstr);

/**
 * @brief Disconnect all clients.
 *
 * @return void
 */
void disconnect_all_clients();

/**
 * @brief Setup the server.
 *
 * @param server_socket_ipv4 The IPv4 server socket.
 * @param server_socket_ipv6 The IPv6 server socket.
 * @param server_addr_ipv4 The IPv4 server address.
 * @param server_addr_ipv6 The IPv6 server address.
 * @return void
 */
void setup_server(int& server_socket_ipv4, int& server_socket_ipv6, sockaddr_in& server_addr_ipv4,
                  sockaddr_in6& server_addr_ipv6);

/**
 * @brief Setup the RocksDB database.
 *
 * @param db The database.
 * @return void
 * @note The database is stored in the DB_PATH directory.
 */
void setup_rocksdb(rocksdb::DB*& db);

#endif
