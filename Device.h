#pragma once
#include <cstddef>

class Device {
public:
    virtual ~Device() = default;
    virtual int open() = 0;
    virtual int close() = 0;
    virtual int read(void* buf, size_t sz) = 0;
    virtual int write(const void* buf, size_t sz) = 0;
    virtual int ioctl(int cmd, void* arg) = 0;
};
