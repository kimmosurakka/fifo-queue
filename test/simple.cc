/**
 * Copyright (c) 2020 Kimmo Surakka
 *
 * Licensed under the MIT license. See file COPYING in the
 * project root for details.
 **/

// Simple unit tests.

#include "gtest/gtest.h"
#include "fifo_queue.hh"

#include <string>
#include <thread>
#include <functional>

void wait_while(std::function<bool()> cond, int maxtries = 50) {
    while(cond() && maxtries-- > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
    EXPECT_EQ(state, WAITING);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(state, WAITING);
    queue << "Food for thought";
    wait_while([&]() {return state == WAITING;});
    EXPECT_EQ(state, FINISHED);
    consumer.join();
}

TEST(Fifo, PoppingFromClosedReturnsWaitingData) {
    fifo_queue<int> queue;
    for (int i=0; i<5; ++i) {
        EXPECT_TRUE(queue << int(i));
    }
    queue.close();
    for (int i=0; i<5; ++i) {
        int val;
        EXPECT_TRUE(queue >> val);
        EXPECT_EQ(val, i);
    }
}

TEST(Fifo, PushingToClosedFails) {
    fifo_queue<std::string> queue;
    queue.close();
    EXPECT_FALSE(queue << "It's a trap!");
}

TEST(Fifo, PushingToOpenSucceeds) {
    fifo_queue<std::string> queue;
    EXPECT_TRUE(queue << "Lucky guess");
}

TEST(Fifo, CreatedQueueIsOpen) {
    fifo_queue<std::string> queue;
    EXPECT_FALSE(queue.is_closed());
}

TEST(Fifo, ClosedSetsIsClosed) {
    fifo_queue<std::string> queue;
    queue.close();
    EXPECT_TRUE(queue.is_closed());
}