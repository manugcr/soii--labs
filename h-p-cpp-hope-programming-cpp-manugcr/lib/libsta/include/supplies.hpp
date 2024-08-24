#ifndef SUPPLIE_DATA_HPP
#define SUPPLIE_DATA_HPP

#include "nlohmann/json.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <rocksdb/db.h>
#include <string>

// Public functions

/**
 * @brief Create the initial state summary and store it in the database.
 *
 * @param db The RocksDB database object.
 * @return void
 */
void create_state_summary(rocksdb::DB* db);

/**
 * @brief Show the state summary in a human-readable format.
 *
 * @param db The RocksDB database object.
 * @param status The status of the RocksDB database.
 * @return std::string The state summary in a human-readable format.
 */
std::string show_state_summary(rocksdb::DB* db, rocksdb::Status status);

/**
 * @brief Get the supplies object from the state summary.
 *
 * @param db The RocksDB database object.
 * @param status The status of the RocksDB database.
 * @return std::string The supplies object in JSON format.
 */
std::string get_supplies(rocksdb::DB* db, rocksdb::Status status);

/**
 * @brief Get the alerts object from the state summary.
 *
 * @param db The RocksDB database object.
 * @param status The status of the RocksDB database.
 * @return std::string The alerts object in JSON format.
 */
std::string get_alerts(rocksdb::DB* db, rocksdb::Status status);

/**
 * @brief Get the emergencies object from the state summary.
 *
 * @param db The RocksDB database object.
 * @param status The status of the RocksDB database.
 * @return std::string The emergencies object in JSON format.
 */
std::string get_emergencies(rocksdb::DB* db, rocksdb::Status status);

/**
 * @brief Modify the supplies object in the state summary.
 *
 * @param db The RocksDB database object.
 * @param supply_key The key of the supply to modify.
 * @param new_value The new value of the supply.
 * @return void
 */
void modify_supplies(rocksdb::DB* db, const std::string& supply_key, const std::string& new_value);

/**
 * @brief Modify the alerts object in the state summary.
 *
 * @param db The RocksDB database object.
 * @param alert_string The alert string to modify.
 * @return void
 */
void modify_alerts(rocksdb::DB* db, const char* alert_string);

/**
 * @brief Modify the emergencies object in the state summary.
 *
 * @param db The RocksDB database object.
 * @param timestamp The timestamp of the emergency event.
 * @param message The message of the emergency event.
 * @return void
 */
void modify_emergencies(rocksdb::DB* db, const char* timestamp, const char* message);

// Private helper functions
namespace
{

/**
 * @brief Create the initial state summary.
 *
 * @return nlohmann::json The initial state summary.
 */
nlohmann::json create_initial_state_summary();

/**
 * @brief Parse the state summary string into a JSON object.
 *
 * @param state_summary_str The state summary string.
 * @return nlohmann::json The state summary in JSON format.
 */
nlohmann::json parse_state_summary(const std::string& state_summary_str);

/**
 * @brief Get a JSON field from a JSON object.
 *
 * @param summary The JSON object.
 * @param field The field to get.
 * @return nlohmann::json The JSON field.
 */
nlohmann::json get_json_field(const nlohmann::json& summary, const std::string& field);

/**
 * @brief Get the value of a key from the database.
 *
 * @param db The RocksDB database object.
 * @param key The key to get.
 * @param value The value to get.
 * @return rocksdb::Status The status of the RocksDB database.
 */
rocksdb::Status get_value(rocksdb::DB* db, const std::string& key, std::string& value);

/**
 * @brief Put a key-value pair into the database.
 *
 * @param db The RocksDB database object.
 * @param key The key to put.
 * @param value The value to put.
 * @return rocksdb::Status The status of the RocksDB database.
 */
rocksdb::Status put_value(rocksdb::DB* db, const std::string& key, const std::string& value);
} // namespace

#endif // SUPPLIES_HPP
