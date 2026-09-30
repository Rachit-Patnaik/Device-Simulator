#pragma once
#include "Device.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <random>

struct SysStats {
    int cpu_load;
    int free_ram_mb;
};

class SystemStatsDevice : public Device {
private:
    bool is_open;
    std::ofstream logf;

    void log_event(const std::string& msg) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string tstr = std::ctime(&now);
        tstr.pop_back();
        logf << "[" << tstr << "] " << msg << "\n";
    }

public:
    SystemStatsDevice() : is_open(false) {
        logf.open("logs/dev_log.txt", std::ios::app);
    }

    ~SystemStatsDevice() {
        if(is_open) close();
    }

    int open() override {
        is_open = true;
        log_event("SYS_STATS OPEN");
        return 0;
    }

    int close() override {
        is_open = false;
        log_event("SYS_STATS CLOSE");
        return 0;
    }

    int read(void* buf, size_t sz) override {
        if(!is_open || sz != sizeof(SysStats)) return -1;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> cpu_dist(5, 98);
        std::uniform_int_distribution<> ram_dist(1024, 16384);

        SysStats* stats = (SysStats*)buf;
        stats->cpu_load = cpu_dist(gen);
        stats->free_ram_mb = ram_dist(gen);

        log_event("READ: CPU " + std::to_string(stats->cpu_load) + "% RAM " + std::to_string(stats->free_ram_mb) + "MB");
        return sizeof(SysStats);
    }

    int write(const void*, size_t) override {
        if(!is_open) return -1;
        log_event("WRITE ERR: Read-only");
        return -1;
    }

    int ioctl(int, void*) override {
        return -1;
    }
};
