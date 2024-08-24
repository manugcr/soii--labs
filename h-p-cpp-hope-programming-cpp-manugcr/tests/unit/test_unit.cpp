#include "alerts.hpp"
#include "emergency.hpp"
#include "supplies.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <rocksdb/db.h>
#include <sstream>

/************************************************************************************
 *                              SUPPLIES TESTS
 *
 ************************************************************************************/
// Utility function to open a RocksDB instance
rocksdb::DB* open_test_db(const std::string& db_path)
{
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
    if (!status.ok())
    {
        std::cerr << "Error opening RocksDB: " << status.ToString() << std::endl;
        return nullptr;
    }
    return db;
}

// Test fixture class for setting up and tearing down the test environment
class SuppliesTest : public ::testing::Test
{
  protected:
    rocksdb::DB* db;
    std::string db_path = "testdb";

    void SetUp() override
    {
        db = open_test_db(db_path);
        ASSERT_NE(db, nullptr);
    }

    void TearDown() override
    {
        delete db;
        rocksdb::DestroyDB(db_path, rocksdb::Options());
    }
};

// Test cases
TEST_F(SuppliesTest, CreateStateSummaryTest)
{
    create_state_summary(db);

    std::string value;
    rocksdb::Status s = db->Get(rocksdb::ReadOptions(), "state_summary", &value);
    ASSERT_TRUE(s.ok());

    nlohmann::json summary = nlohmann::json::parse(value);
    ASSERT_EQ(summary["alerts"]["north_entry"], 0);
    ASSERT_EQ(summary["supplies"]["food"]["meat"], 100);
}

TEST_F(SuppliesTest, ShowStateSummaryTest)
{
    create_state_summary(db);

    std::string summary = show_state_summary(db, rocksdb::Status::OK());
    ASSERT_FALSE(summary.empty());

    nlohmann::json json_summary = nlohmann::json::parse(summary);
    ASSERT_EQ(json_summary["alerts"]["north_entry"], 0);
}

TEST_F(SuppliesTest, GetSuppliesTest)
{
    create_state_summary(db);

    std::string supplies = get_supplies(db, rocksdb::Status::OK());
    ASSERT_FALSE(supplies.empty());

    nlohmann::json json_supplies = nlohmann::json::parse(supplies);
    ASSERT_EQ(json_supplies["food"]["meat"], 100);
}

TEST_F(SuppliesTest, ModifySuppliesTest)
{
    create_state_summary(db);

    modify_supplies(db, "meat", "50");
    std::string supplies = get_supplies(db, rocksdb::Status::OK());
    nlohmann::json json_supplies = nlohmann::json::parse(supplies);
    ASSERT_EQ(json_supplies["food"]["meat"], 50);
}

/************************************************************************************
 *                              EMGS TESTS
 *
 ************************************************************************************/
TEST(EmergencyTest, OpenEmergencyPipe_Success)
{
    const std::string test_pipe = "test_emergency_pipe.txt";
    std::ofstream ofs(test_pipe);
    ofs.close();

    std::ofstream emgs_pipe;
    emgs_pipe.open(test_pipe);
    if (!emgs_pipe.is_open())
    {
        std::cerr << "[ERROR] Error opening test emergency pipe" << std::endl;
    }
    bool result = emgs_pipe.is_open();

    EXPECT_TRUE(result);
    EXPECT_TRUE(emgs_pipe.is_open());

    emgs_pipe.close();
    std::remove(test_pipe.c_str());
}

TEST(EmergencyTest, OpenEmergencyPipe_Failure)
{
    const std::string invalid_path = "/invalid/path/to/pipe.txt";
    std::ofstream emgs_pipe;

    // Temporarily change the EMGS_PIPE macro for this test
    const std::string original_emgs_pipe = EMGS_PIPE;
#undef EMGS_PIPE
#define EMGS_PIPE invalid_path

    bool result = open_emergency_pipe(emgs_pipe);

    EXPECT_FALSE(result);
    EXPECT_FALSE(emgs_pipe.is_open());

// Restore the original EMGS_PIPE macro
#undef EMGS_PIPE
#define EMGS_PIPE original_emgs_pipe
}

TEST(EmergencyTest, GenerateEmergencyMessage)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::string message = generate_emergency_message(gen);
    EXPECT_TRUE(message == "Power Outage" || message == "Server Overload" || message == "Network Disruption" ||
                message == "System Failure");
}

TEST(EmergencyTest, WriteEmergencyMessage)
{
    std::string message = "Power Outage";

    std::stringstream ss;
    write_emergency_message(ss, message);

    std::string expected_output = "Power Outage\n";
    EXPECT_EQ(ss.str(), expected_output);
}

/************************************************************************************
 *                              ALERTS TESTS
 *
 ************************************************************************************/
TEST(AlertsTest, OpenAlertPipe_Success)
{
    const std::string test_pipe = "test_alert_pipe.txt";
    std::ofstream ofs(test_pipe);
    ofs.close();

    std::ofstream alert_pipe;
    alert_pipe.open(test_pipe);
    if (!alert_pipe.is_open())
    {
        std::cerr << "[ERROR] Error opening test alert pipe" << std::endl;
    }
    bool result = alert_pipe.is_open();

    EXPECT_TRUE(result);
    EXPECT_TRUE(alert_pipe.is_open());

    alert_pipe.close();
    std::remove(test_pipe.c_str());
}

TEST(AlertsTest, OpenAlertPipe_Failure)
{
    const std::string invalid_path = "/invalid/path/to/pipe.txt";
    std::ofstream alert_pipe;

    // Temporarily change the ALERT_PIPE macro for this test
    const std::string original_alert_pipe = ALERT_PIPE;
#undef ALERT_PIPE
#define ALERT_PIPE invalid_path

    bool result = open_alert_pipe(alert_pipe);

    EXPECT_FALSE(result);
    EXPECT_FALSE(alert_pipe.is_open());

// Restore the original ALERT_PIPE macro
#undef ALERT_PIPE
#define ALERT_PIPE original_alert_pipe
}

TEST(AlertsTest, GenerateEntryPoint)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::string entry = generate_entry_point(gen);
    EXPECT_TRUE(entry == "north" || entry == "east" || entry == "west" || entry == "south");
}

TEST(AlertsTest, GenerateTemperature)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    double temp = generate_temperature(gen);
    EXPECT_GE(temp, 36.0);
    EXPECT_LE(temp, 42.0);
}

TEST(AlertsTest, GenerateSleepDuration)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int duration = generate_sleep_duration(gen);
    EXPECT_GE(duration, 10);
    EXPECT_LE(duration, 30);
}

TEST(AlertsTest, WriteAlert_TemperatureAboveThreshold)
{
    std::string entry = "north";
    double temperature = 39.5;

    std::stringstream ss;
    write_alert(ss, entry, temperature);

    std::string expected_output = "north,39.5\n";
    EXPECT_EQ(ss.str(), expected_output);
}

TEST(AlertsTest, WriteAlert_TemperatureBelowThreshold)
{
    std::string entry = "south";
    double temperature = 37.0;

    std::stringstream ss;
    write_alert(ss, entry, temperature);

    std::string expected_output = "";
    EXPECT_EQ(ss.str(), expected_output);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
