#include "gtest/gtest.h"
#include "fifo_queue.hh"

#include <string>
#include <thread>
#include <functional>

void wait_while(std::function<bool()> cond, int maxtries = 50) {
    while(cond() && maxtries-- > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

TEST(Fifo, PoppingReturnsPushed)
{
    fifo_queue<std::string> queue;
    queue << "Garbage in";
    std::string out;
    queue >> out;
    EXPECT_EQ(out, "Garbage in");
}

TEST(Fifo, PoppingFromEmptyBlocks)
{
    fifo_queue<std::string> queue;
    enum {NOT_STARTED, WAITING, FINISHED} state = NOT_STARTED;

    auto consumer = std::thread([&]() {
        state = WAITING;
        std::string out;
        queue >> out;
        state = FINISHED;
    });
    wait_while([&]() {return state == NOT_STARTED;});
    ASSERT_EQ(state, WAITING);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_EQ(state, WAITING);
    queue << "Food for thought";
    wait_while([&]() {return state == WAITING;});
    ASSERT_EQ(state, FINISHED);
    consumer.join();
}

TEST(Fifo, PoppingFromClosedReturnsWaitingData) {
    fifo_queue<int> queue;
    for (int i=0; i<5; ++i) {
        ASSERT_TRUE(queue << int(i));
    }
    queue.close();
    for (int i=0; i<5; ++i) {
        int val;
        ASSERT_TRUE(queue >> val);
        ASSERT_EQ(val, i);
    }
    int val;
    ASSERT_FALSE(queue >> val);
}
