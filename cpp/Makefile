CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

HEADERS  = mask.hpp grid.hpp agent.hpp simulator.hpp

# Executables
MIS_TARGET   = mis_simulator
MASS_TARGET  = mass_simulate
VIS_TARGET   = visualizer_logger

# Sources for each executable
MIS_SRC   = main.cpp
MASS_SRC  = analyse.cpp
VIS_SRC   = visualizer.cpp    # <-- change if you used a different file name

ALL_TARGETS = $(MIS_TARGET) $(MASS_TARGET) $(VIS_TARGET)

all: $(ALL_TARGETS)

$(MIS_TARGET): $(MIS_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(MIS_SRC) -o $(MIS_TARGET)

$(MASS_TARGET): $(MASS_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(MASS_SRC) -o $(MASS_TARGET)

$(VIS_TARGET): $(VIS_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(VIS_SRC) -o $(VIS_TARGET)

clean:
	rm -f $(ALL_TARGETS)

run_mis: $(MIS_TARGET)
	./$(MIS_TARGET) 5 5 v

run_mass: $(MASS_TARGET)
	./$(MASS_TARGET) 20 20 100

run_vis: $(VIS_TARGET)
	./$(VIS_TARGET) 20 20 1000

.PHONY: all clean run_mis run_mass run_vis
