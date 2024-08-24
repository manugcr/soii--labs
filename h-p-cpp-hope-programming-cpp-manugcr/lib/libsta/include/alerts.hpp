#ifndef ALERTS_HPP
#define ALERTS_HPP

#include <chrono>
#include <fcntl.h>
#include <fstream>
#include <iomanip> // For std::fixed, std::setprecision
#include <iostream>
#include <random>
#include <sstream> // For std::stringstream
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

#define ALERT_PIPE "../tmp/alert_pipe"

/**
 * @brief Open the alert pipe.
 *
 * This function opens the alert pipe for writing.
 *
 * @param alert_pipe The alert pipe to open.
 * @return bool True if the alert pipe was opened successfully, false otherwise.
 */
bool open_alert_pipe(std::ofstream& alert_pipe);

/**
 * @brief Generate a random entry point.
 *
 * This function generates a random entry point for an invasion.
 *
 * @param gen The random number generator.
 * @return std::string The randomly generated entry point.
 */
std::string generate_entry_point(std::mt19937& gen);

/**
 * @brief Generate a random temperature.
 *
 * This function generates a random temperature between 36.0 and 42.0.
 *
 * @param gen The random number generator.
 * @return double The randomly generated temperature.
 */
double generate_temperature(std::mt19937& gen);

/**
 * @brief Generate a random sleep duration.
 *
 * This function generates a random sleep duration between 10 and 30 seconds.
 *
 * @param gen The random number generator.
 * @return int The randomly generated sleep duration.
 */
int generate_sleep_duration(std::mt19937& gen);

/**
 * @brief Write an alert to the alert pipe.
 *
 * This function writes an alert to the alert pipe if the temperature is above 38.0.
 *
 * @param alert_pipe The alert pipe to write to.
 * @param entry The entry point of the invasion.
 * @param temperature The temperature at the entry point.
 * @return void
 */
void write_alert(std::ostream& alert_pipe, const std::string& entry, double temperature);

/**
 * @brief Run the invasion alerts.
 *
 * This function generates random invasion alerts and writes them to the named pipe.
 *
 * @return void
 */
void run_invasion_alerts();

#endif // ALERTS_HPP
