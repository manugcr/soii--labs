#ifndef EMERGENCY_HPP
#define EMERGENCY_HPP

#include <chrono>
#include <fcntl.h>
#include <fstream>
#include <iomanip> // For std::fixed, std::setprecision
#include <iostream>
#include <random>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

#define EMGS_PIPE "../tmp/emgs_pipe"

/**
 * @brief Open the named pipe for writing.
 *
 * @param emgs_pipe The output stream to the named pipe.
 * @return bool True if the pipe was opened successfully, false otherwise.
 */
bool open_emergency_pipe(std::ofstream& emgs_pipe);

/**
 * @brief Generate a random emergency message.
 *
 * @param gen The random number generator.
 * @return std::string The generated emergency message.
 */
std::string generate_emergency_message(std::mt19937& gen);

/**
 * @brief Write an emergency message to the named pipe.
 *
 * @param emgs_pipe The output stream to the named pipe.
 * @param emgs_message The emergency message to write.
 * @return void
 */
void write_emergency_message(std::ostream& emgs_pipe, const std::string& emgs_message);

/**
 * @brief Run the emergencies.
 *
 * This function generates random emergencies and writes them to the named pipe.
 *
 * @return void
 */
void run_emergencies();

#endif
