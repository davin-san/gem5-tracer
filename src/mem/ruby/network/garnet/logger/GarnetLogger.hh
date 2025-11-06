#pragma once
#include <cstdint>
#include <fstream>
#include <memory>
#include <mutex>
#include <vector>

#include "mem/ruby/network/garnet/proto/garnet_event.pb.h"

class GarnetLogger
{
public:
    static GarnetLogger& instance();

    void logEvent(const garnetlog::GarnetEvent &event);

    void flush();

    void setFlushThreshold(size_t msgs);

    // destructor flushes remaining
    ~GarnetLogger();

    GarnetLogger(const GarnetLogger&) = delete;
    GarnetLogger& operator=(const GarnetLogger&) = delete;

private:
    GarnetLogger();
    void writeBufferToDisk();

    std::mutex mtx;
    std::vector<std::string> buffer;
    size_t flushThreshold;
    std::ofstream out;
};
