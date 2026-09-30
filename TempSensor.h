#pragma once
#include "Device.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <random>

#define IOCTL_SET_THRESHOLD 1

class TempSensor : public Device {
private:
    bool is_open;
    int thresh;
    bool led_on;
    std::ofstream logf;

    void log_event(const std::string& msg) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string tstr = std::ctime(&now);
        tstr.pop_back();
        logf << "[" << tstr << "] " << msg << "\n";
    }

public:
    TempSensor() : is_open(false), thresh(100), led_on(false) {
        logf.open("logs/dev_log.txt", std::ios::app);
    }

    ~TempSensor() {
        if(is_open) close();
    }

    int open() override {
        is_open = true;
        log_event("DEV OPEN");
        return 0;
    }

    int close() override {
        is_open = false;
        log_event("DEV CLOSE");
        return 0;
    }

    int read(void* buf, size_t sz) override {
        if(!is_open || sz != sizeof(int)) return -1;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(25, 45);

        int t = dist(gen);
        *(int*)buf = t;

        log_event("READ: " + std::to_string(t) + "C");

        if(t > thresh && !led_on) {
            led_on = true;
            log_event("ALERT: LED ON");
            std::cout << "[HW] LED ON - Hot!\n";
        } else if(t <= thresh && led_on) {
            led_on = false;
            log_event("INFO: LED OFF");
            std::cout << "[HW] LED OFF - Normal\n";
        }

        return sizeof(int);
    }

    int write(const void*, size_t) override {
        if(!is_open) return -1;
        log_event("WRITE ERR: Read-only");
        return -1;
    }

    int ioctl(int cmd, void* arg) override {
        if(!is_open || !arg) return -1;

        if(cmd == IOCTL_SET_THRESHOLD) {
            thresh = *(int*)arg;
            log_event("IOCTL: thresh=" + std::to_string(thresh));
            return 0;
        }
        return -1;
    }
};
