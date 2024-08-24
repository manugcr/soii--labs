#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <rocksdb/db.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <zlib.h>

#define TRUE 1
#define MAX 1024
#define SA struct sockaddr
#define ZIP_FILE "./tmp/received_file.zip"
#define RECV_IMG "./tmp/received_file.png"

int PORT;
const char* ADDR;

/**
 * @brief Decompress a file.
 *
 * @param input_path_zip The input path of the zip file.
 * @param output_path_png The output path of the png file.
 * @return bool
 */
bool decompress_file(const std::string& input_path_zip, const std::string& output_path_jpg);

/**
 * @brief Receive a compressed file from the server.
 *
 * @param sockfd The socket file descriptor.
 * @param buffer The buffer to store the received data.
 * @return bool
 */
bool receive_compressed_file(int sockfd, char buffer[MAX]);

#endif
