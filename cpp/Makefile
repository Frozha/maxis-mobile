CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
TARGET = mis_simulator
SOURCES = main.cpp
HEADERS = mask.hpp grid.hpp agent.hpp simulator.hpp

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET) 5 5 v

.PHONY: all clean run