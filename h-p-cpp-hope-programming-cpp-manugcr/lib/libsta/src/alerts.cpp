#include "alerts.hpp"

bool open_alert_pipe(std::ofstream& alert_pipe)
{
    alert_pipe.open(ALERT_PIPE);
    if (!alert_pipe.is_open())
    {
        std::cerr << "[ERROR] Error opening alert pipe" << std::endl;
        return false;
    }
    return true;
}

std::string generate_entry_point(std::mt19937& gen)
{
    std::uniform_int_distribution<> entry_dist(0, 3);
    int entry_point = entry_dist(gen);
    switch (entry_point)
    {
    case 0:
        return "north";
    case 1:
        return "east";
    case 2:
        return "west";
    case 3:
        return "south";
    default:
        return "";
    }
}

double generate_temperature(std::mt19937& gen)
{
    std::uniform_real_distribution<> temp_dist(36.0, 42.0);
    return temp_dist(gen);
}

int generate_sleep_duration(std::mt19937& gen)
{
    std::uniform_int_distribution<> dis(10, 30);
    return dis(gen);
}

void write_alert(std::ostream& alert_pipe, const std::string& entry, double temperature)
{
    if (temperature > 38.0)
    {
        std::stringstream temperature_stream;
        temperature_stream << std::fixed << std::setprecision(1) << temperature;
        std::string alert_message = entry + "," + temperature_stream.str();
        alert_pipe << alert_message << std::endl;
        alert_pipe.flush();
    }
}

void run_invasion_alerts()
{
    std::ofstream alert_pipe;
    if (!open_alert_pipe(alert_pipe))
        return;

    std::random_device rd;
    std::mt19937 gen(rd());

    while (true)
    {
        int time = generate_sleep_duration(gen);
        std::this_thread::sleep_for(std::chrono::seconds(time));

        std::string entry = generate_entry_point(gen);
        double temperature = generate_temperature(gen);

        write_alert(alert_pipe, entry, temperature);
    }

    alert_pipe.close();
}
