#include "emergency.hpp"

bool open_emergency_pipe(std::ofstream& emgs_pipe)
{
    emgs_pipe.open(EMGS_PIPE);
    if (!emgs_pipe.is_open())
    {
        std::cerr << "[ERROR] Error opening emergency pipe" << std::endl;
        return false;
    }
    return true;
}

std::string generate_emergency_message(std::mt19937& gen)
{
    std::uniform_int_distribution<> dis(0, 3);
    int emergency_type = dis(gen);
    switch (emergency_type)
    {
    case 0:
        return "Power Outage";
    case 1:
        return "Server Overload";
    case 2:
        return "Network Disruption";
    case 3:
        return "System Failure";
    default:
        return "Unknown Emergency";
    }
}

void write_emergency_message(std::ostream& emgs_pipe, const std::string& emgs_message)
{
    emgs_pipe << emgs_message << std::endl;
    emgs_pipe.flush();
}

void run_emergencies()
{
    // Open the named pipe for writing
    std::ofstream emgs_pipe;
    if (!open_emergency_pipe(emgs_pipe))
        return;

    // Define random number generators
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(60, 90);

    while (true)
    {
        int time = dis(gen);
        std::this_thread::sleep_for(std::chrono::seconds(time));

        // Generate a random emergency message
        std::string emgs_message = generate_emergency_message(gen);

        // Write the emergency to the named pipe
        write_emergency_message(emgs_pipe, emgs_message);
    }

    // The pipe will be closed automatically when the ofstream goes out of scope
}
