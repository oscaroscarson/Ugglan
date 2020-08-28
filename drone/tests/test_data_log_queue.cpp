#include <catch.h>

#include <thread>
#include <cstring>
#include <task.h>
#include <wall_time.h>
#include <data_log_queue.h>

static const double FLOAT_TOL = 1e-4;

DataLogQueue data_log_queue_multi;

class TestTaskOne : public Task
{
public:
    TestTaskOne(uint32_t exec_period_ms, void (*exec_period_exceeded_cb)()) :
        Task(exec_period_ms, exec_period_exceeded_cb) {}
protected:
    void _execute() { data_log_queue_multi.push(1.0, DataLogSignal::ImuAccelerationX); }
};

class TestTaskTwo : public Task
{
public:
    TestTaskTwo(uint32_t exec_period_ms, void (*exec_period_exceeded_cb)()) :
        Task(exec_period_ms, exec_period_exceeded_cb) {}
protected:
    void _execute() { data_log_queue_multi.push(2.0, DataLogSignal::ImuAccelerationY); }
};

TEST_CASE("data_log_queue: single thread")
{
    DataLogQueue data_log_queue;

    SECTION("is empty")
    {
        REQUIRE(data_log_queue.is_empty() == true);
    }
    SECTION("push and pop")
    {
        double data = -3.14;
        const uint8_t sleep_ms = 10;

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));

        data_log_queue.push(data, DataLogSignal::ImuAccelerationX);
        REQUIRE(data_log_queue.is_empty() == false);

        DataLogSample sample = data_log_queue.pop();
        REQUIRE(data_log_queue.is_empty() == true);

        REQUIRE(std::memcmp(&data, &sample.data, sizeof(double)) == 0);
        REQUIRE(sample.rel_timestamp_ms >= sleep_ms);
        REQUIRE(sample.type == DataLogType::DOUBLE);
        REQUIRE(sample.signal == DataLogSignal::ImuAccelerationX);
    }
}

TEST_CASE("data_log_queue: multi thread")
{
    TestTaskOne task_one(10, nullptr); // Runs at 100 Hz
    TestTaskTwo task_two(20, nullptr); // Runs at 50 Hz

    task_one.launch();
    task_two.launch();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    task_one.teardown();
    task_two.teardown();

    REQUIRE(data_log_queue_multi.is_empty() == false);

    uint8_t n_act_one_samples = 0;
    uint8_t n_act_two_samples = 0;

    double exp_one_sample_data = 1.0;
    double exp_two_sample_data = 2.0;

    while (!data_log_queue_multi.is_empty())
    {
        DataLogSample sample = data_log_queue_multi.pop();

        if ((std::memcmp(&exp_one_sample_data, &sample.data, sizeof(double)) == 0) &&
             sample.signal == DataLogSignal::ImuAccelerationX)
        {
            n_act_one_samples++;
        }
        else if ((std::memcmp(&exp_two_sample_data, &sample.data, sizeof(double)) == 0) &&
                  sample.signal == DataLogSignal::ImuAccelerationY)
        {
            n_act_two_samples++;
        }
        else
        {
            FAIL("Unexpected sample");
        }
    }

    REQUIRE(n_act_one_samples >= 10);
    REQUIRE(n_act_two_samples >= 5);
}

TEST_CASE("data_log_queue: error handling")
{
    DataLogQueue data_log_queue;

    SECTION("unsupported type")
    {
        bool data;
        REQUIRE_THROWS_WITH(data_log_queue.push(data, DataLogSignal::ImuAccelerationX),
                            "Unsupported data type");
    }
    SECTION("signal type missmatch")
    {
        uint8_t data;
        REQUIRE_THROWS_WITH(data_log_queue.push(data, DataLogSignal::ImuAccelerationX),
                            "Data log signal type missmatch");
    }
    SECTION("timestamp overflow")
    {
        double data;
        std::this_thread::sleep_for(std::chrono::milliseconds(UINT8_MAX + 1));
        REQUIRE_THROWS_WITH(data_log_queue.push(data, DataLogSignal::ImuAccelerationX),
                            "Data log queue timstamp overflow. Called too seldom");
    }
}