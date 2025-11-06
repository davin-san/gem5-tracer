#include "mem/ruby/network/garnet/logger/GarnetLogger.hh"

#include <iostream>

// singleton
GarnetLogger& GarnetLogger::instance() {
    static GarnetLogger inst;
    return inst;
}

GarnetLogger::GarnetLogger()
    : flushThreshold(1024)
{
    // 二进制追加打开一个文件
    out.open("/workspace/garnet-web-visualizer/traces/garnet_event_log.bin",
        std::ios::binary | std::ios::app);
    if (!out.is_open()) {
        std::cerr <<
        "Failed to open garnet_event_log.bin for writing\n";
    }
}

GarnetLogger::~GarnetLogger() {
    flush();
    if (out.is_open()) out.close();
}

void GarnetLogger::setFlushThreshold(size_t msgs) {
    std::lock_guard<std::mutex> lock(mtx);
    flushThreshold = msgs;
}

void GarnetLogger::logEvent(const garnetlog::GarnetEvent &event) {
    std::string bytes;
    event.SerializeToString(&bytes);

    {
        std::lock_guard<std::mutex> lock(mtx);
        buffer.push_back(std::move(bytes));
        if (buffer.size() >= flushThreshold) {
            writeBufferToDisk();
        }
    }
}

void GarnetLogger::writeBufferToDisk() {
    if (!out.is_open()) return;
    for (const auto &b : buffer) {
        uint32_t len = static_cast<uint32_t>(b.size());
        out.write(reinterpret_cast<const char *>(&len), sizeof(len));
        out.write(b.data(), b.size());
    }
    out.flush();
    buffer.clear();
}

void GarnetLogger::flush() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!buffer.empty()) {
        writeBufferToDisk();
    }
}
