#include "TempSensor.h"
#include "SystemStatsDevice.h"
#include <iostream>
#include <thread>
#include <csignal>
#include <atomic>

// Global pointers for clean shutdown via signal handler
Device* temp_dev = nullptr;
Device* sys_dev = nullptr;
std::atomic<bool> keep_running(true);

void sigint_handler(int signum) {
    std::cout << "\n[SIGINT] Interrupt signal (" << signum << ") received.\n";
    std::cout << "Initiating clean shutdown of all hardware devices...\n";
    keep_running = false;
}

int main() {
    std::signal(SIGINT, sigint_handler);
    std::cout << "Initializing system devices (Press Ctrl+C to exit)...\n\n";
    
    temp_dev = new TempSensor();
    sys_dev = new SystemStatsDevice();
    
    temp_dev->open();
    sys_dev->open();
    
    int th = 38;
    temp_dev->ioctl(IOCTL_SET_THRESHOLD, &th);
    
    // Demonstrate Error Path
    std::cout << ">>> DEMO: Testing Error Path (Writing to Read-Only TempSensor)...\n";
    int dummy = 99;
    if (temp_dev->write(&dummy, sizeof(dummy)) == -1) {
        std::cout << ">>> RESULT: Write blocked by driver. Check logs for details.\n";
    }
    std::cout << "------------------------\n";
    
    int temp_val = 0;
    SysStats stats_val;
    
    int cycles = 0;
    while(keep_running && cycles++ < 5) {
        temp_dev->read(&temp_val, sizeof(temp_val));
        std::cout << "Temp Sensor: " << temp_val << "C\n";
        
        sys_dev->read(&stats_val, sizeof(stats_val));
        std::cout << "System Load: " << stats_val.cpu_load 
                  << "% CPU | " << stats_val.free_ram_mb << " MB Free RAM\n";
        
        std::cout << "------------------------\n";
        
        // Sleep in 100ms chunks so Ctrl+C feels responsive
        for(int s=0; s<10 && keep_running; s++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    // Clean shutdown sequence triggered by SIGINT or normal exit
    if (temp_dev) { temp_dev->close(); delete temp_dev; }
    if (sys_dev) { sys_dev->close(); delete sys_dev; }
    
    std::cout << "System offline. Devices closed successfully.\n";
    return 0;
}
