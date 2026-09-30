CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
TARGET = sim

all: $(TARGET)

$(TARGET): main.cpp TempSensor.h SystemStatsDevice.h Device.h
	mkdir -p logs
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

clean:
	rm -f $(TARGET)
	rm -rf logs
