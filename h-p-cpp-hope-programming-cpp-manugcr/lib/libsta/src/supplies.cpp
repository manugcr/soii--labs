#include "supplies.hpp"

// Private helper function implementations
namespace
{
nlohmann::json create_initial_state_summary()
{
    nlohmann::json state_summary;
    state_summary["alerts"]["north_entry"] = 0;
    state_summary["alerts"]["east_entry"] = 0;
    state_summary["alerts"]["west_entry"] = 0;
    state_summary["alerts"]["south_entry"] = 0;

    state_summary["supplies"]["food"] = {{"meat", 100}, {"vegetables", 100}, {"fruits", 100}, {"water", 100}};

    state_summary["supplies"]["medicine"] = {{"antibiotics", 100}, {"analgesics", 100}, {"bandages", 100}};

    state_summary["emergency"]["last_keepalived"] = "NONE";
    state_summary["emergency"]["last_event"] = "NONE";

    return state_summary;
}

nlohmann::json parse_state_summary(const std::string& state_summary_str)
{
    return nlohmann::json::parse(state_summary_str);
}

nlohmann::json get_json_field(const nlohmann::json& summary, const std::string& field)
{
    return summary[field];
}

rocksdb::Status get_value(rocksdb::DB* db, const std::string& key, std::string& value)
{
    return db->Get(rocksdb::ReadOptions(), key, &value);
}

rocksdb::Status put_value(rocksdb::DB* db, const std::string& key, const std::string& value)
{
    return db->Put(rocksdb::WriteOptions(), key, value);
}
} // namespace

// Public function implementations

void create_state_summary(rocksdb::DB* db)
{
    nlohmann::json state_summary = create_initial_state_summary();
    std::string state_summary_str = state_summary.dump(4);
    rocksdb::Status s = put_value(db, "state_summary", state_summary_str);
    if (!s.ok())
    {
        std::cerr << "[ERROR] Error creating state summary in database: " << s.ToString() << std::endl;
    }
}

std::string show_state_summary(rocksdb::DB* db, rocksdb::Status status)
{
    if (!status.ok())
    {
        std::cerr << "Error opening database: " << status.ToString() << std::endl;
        EXIT_FAILURE;
    }

    std::string supplie_string;
    rocksdb::Status s = get_value(db, "state_summary", supplie_string);
    if (!s.ok())
    {
        std::cerr << "Error retrieving value for key state_summary: " << s.ToString() << std::endl;
    }

    nlohmann::json result = parse_state_summary(supplie_string);
    return result.dump(4);
}

std::string get_supplies(rocksdb::DB* db, rocksdb::Status status)
{
    std::string value;
    rocksdb::Status s = get_value(db, "state_summary", value);
    if (!s.ok())
    {
        std::cerr << "[ERROR] Error retrieving value for 'state_summary': " << s.ToString() << std::endl;
    }

    nlohmann::json summary = parse_state_summary(value);
    nlohmann::json supplies = get_json_field(summary, "supplies");
    return supplies.dump(4);
}

std::string get_alerts(rocksdb::DB* db, rocksdb::Status status)
{
    std::string value;
    rocksdb::Status s = get_value(db, "state_summary", value);
    if (!s.ok())
    {
        std::cerr << "[ERROR] Error retrieving value for 'state_summary': " << s.ToString() << std::endl;
    }

    nlohmann::json summary = parse_state_summary(value);
    nlohmann::json alerts = get_json_field(summary, "alerts");
    return alerts.dump(4);
}

std::string get_emergencies(rocksdb::DB* db, rocksdb::Status status)
{
    std::string value;
    rocksdb::Status s = get_value(db, "state_summary", value);
    if (!s.ok())
    {
        std::cerr << "[ERROR] Error retrieving value for 'state_summary': " << s.ToString() << std::endl;
    }

    nlohmann::json summary = parse_state_summary(value);
    nlohmann::json emergencies = get_json_field(summary, "emergency");
    return emergencies.dump(4);
}

void modify_supplies(rocksdb::DB* db, const std::string& supply_key, const std::string& new_value)
{
    std::string value;
    rocksdb::Status s = get_value(db, "state_summary", value);
    if (!s.ok())
    {
        std::cerr << "[ERROR] Error retrieving value for 'state_summary': " << s.ToString() << std::endl;
        return;
    }

    nlohmann::json summary = parse_state_summary(value);
    if (summary.contains("supplies"))
    {
        bool supply_found = false;
        for (auto& category : {"food", "medicine"})
        {
            if (summary["supplies"].contains(category) && summary["supplies"][category].contains(supply_key))
            {
                int value_int;
                try
                {
                    value_int = std::stoi(new_value);
                }
                catch (const std::invalid_argument& e)
                {
                    std::cerr << "[ERROR] Invalid value entered for supply: " << new_value << std::endl;
                    return;
                }

                if (value_int < 0)
                {
                    std::cerr << "[ERROR] Value must be greater than or equal to 0." << std::endl;
                    return;
                }

                summary["supplies"][category][supply_key] = value_int;

                std::string new_summary = summary.dump(4);
                rocksdb::Status s2 = put_value(db, "state_summary", new_summary);
                if (!s2.ok())
                {
                    std::cerr << "[ERROR] Error updating state summary: " << s2.ToString() << std::endl;
                    return;
                }

                std::cout << "[SUPPLIES] State summary updated successfully." << std::endl;
                supply_found = true;
                break;
            }
        }

        if (!supply_found)
        {
            std::cerr << "[ERROR] Supply '" << supply_key << "' not found." << std::endl;
        }
    }
    else
    {
        std::cerr << "[ERROR] 'supplies' object not found in state summary." << std::endl;
    }
}


void modify_alerts(rocksdb::DB* db, const char* alert_string)
{
    std::string value;
    rocksdb::Status s = get_value(db, "state_summary", value);
    if (!s.ok())
    {
        std::cerr << "[ERROR] Error retrieving value for 'state_summary': " << s.ToString() << std::endl;
        return;
    }

    nlohmann::json summary = parse_state_summary(value);
    nlohmann::json alerts = get_json_field(summary, "alerts");

    std::string entry;
    if (strcmp(alert_string, "north") == 0)
    {
        entry = "north_entry";
    }
    else if (strcmp(alert_string, "south") == 0)
    {
        entry = "south_entry";
    }
    else if (strcmp(alert_string, "east") == 0)
    {
        entry = "east_entry";
    }
    else if (strcmp(alert_string, "west") == 0)
    {
        entry = "west_entry";
    }
    else
    {
        std::cerr << "[ERROR] Unrecognized alert string: " << alert_string << std::endl;
        return;
    }

    if (!alerts[entry].is_null())
    {
        alerts[entry] = alerts[entry].get<int>() + 1;
    }
    else
    {
        std::cerr << "[ERROR] Alert value is null" << std::endl;
        exit(EXIT_FAILURE);
    }

    summary["alerts"] = alerts;

    std::string new_summary = summary.dump(4);
    rocksdb::Status s2 = put_value(db, "state_summary", new_summary);
    if (!s2.ok())
    {
        std::cerr << "[ERROR] Error updating state summary: " << s2.ToString() << std::endl;
    }
}

void modify_emergencies(rocksdb::DB* db, const char* timestamp, const char* message)
{
    std::string value;
    rocksdb::Status s = get_value(db, "state_summary", value);
    if (!s.ok())
    {
        std::cerr << "[ERROR] Error retrieving value for 'state_summary': " << s.ToString() << std::endl;
        return;
    }

    nlohmann::json summary = parse_state_summary(value);
    nlohmann::json emergencies = get_json_field(summary, "emergency");

    emergencies["last_keepalived"] = timestamp;

    std::string cleaned_message = message;
    cleaned_message.erase(std::remove(cleaned_message.begin(), cleaned_message.end(), '\n'), cleaned_message.end());
    emergencies["last_event"] = cleaned_message;

    summary["emergency"] = emergencies;

    std::string new_summary = summary.dump(4);
    rocksdb::Status s2 = put_value(db, "state_summary", new_summary);
    if (!s2.ok())
    {
        std::cerr << "[ERROR] Error updating state summary: " << s2.ToString() << std::endl;
    }
}
