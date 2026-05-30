CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

HEADERS          = headers/mask.hpp headers/grid.hpp headers/agent.hpp headers/simulator.hpp
MOD_ALG_HEADERS  = $(HEADERS) modified_alg/CollisionSimulator.hpp
NO_SUP_HEADERS   = $(HEADERS) no_suppression_alg/NoSuppressionSimulator.hpp

# Executables
MIS_TARGET   = mis_simulator
MASS_TARGET  = mass_simulate
VIS_TARGET   = visualizer_log

MOD_ALG_MIS_TARGET  = mod_alg_mis_simulator
MOD_ALG_MASS_TARGET = mod_alg_mass_simulate

MOD_ST_MIS_TARGET  = mod_start_mis_simulator
MOD_ST_MASS_TARGET = mod_start_mass_simulator

NO_SUP_MIS_TARGET  = no_sup_mis_simulator
NO_SUP_MASS_TARGET = no_sup_mass_simulate

CHECK_OVERLAP_TARGET = check_overlap

# Sources for each executable
MIS_SRC   = unmodified_alg/mis_simulator.cpp
MASS_SRC  = unmodified_alg/mass_simulator.cpp
VIS_SRC   = unmodified_alg/visualizer_feeder.cpp

MOD_ST_MIS_SRC  = modified_start/modified_start_mis_simulator.cpp
MOD_ST_MASS_SRC = modified_start/modified_start_mass_simulator.cpp

MOD_ALG_MIS_SRC  = modified_alg/mod_mis_simulator.cpp
MOD_ALG_MASS_SRC = modified_alg/mod_mass_simulator.cpp

NO_SUP_MIS_SRC  = no_suppression_alg/no_suppression_mis_simulator.cpp
NO_SUP_MASS_SRC = no_suppression_alg/no_suppression_mass_simulator.cpp

CHECK_OVERLAP_SRC = check_overlap.cpp

ALL_TARGETS = $(MIS_TARGET) $(MASS_TARGET) $(VIS_TARGET) \
              $(MOD_ALG_MIS_TARGET) $(MOD_ALG_MASS_TARGET) \
              $(MOD_ST_MIS_TARGET) $(MOD_ST_MASS_TARGET) \
              $(NO_SUP_MIS_TARGET) $(NO_SUP_MASS_TARGET) \
              $(CHECK_OVERLAP_TARGET)

all: $(ALL_TARGETS)

$(MIS_TARGET): $(MIS_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(MIS_SRC) -o $(MIS_TARGET)

$(MASS_TARGET): $(MASS_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(MASS_SRC) -o $(MASS_TARGET)

$(VIS_TARGET): $(VIS_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(VIS_SRC) -o $(VIS_TARGET)

$(MOD_ST_MIS_TARGET): $(MOD_ST_MIS_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(MOD_ST_MIS_SRC) -o $(MOD_ST_MIS_TARGET)

$(MOD_ST_MASS_TARGET): $(MOD_ST_MASS_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(MOD_ST_MASS_SRC) -o $(MOD_ST_MASS_TARGET)

$(MOD_ALG_MIS_TARGET): $(MOD_ALG_MIS_SRC) $(MOD_ALG_HEADERS)
	$(CXX) $(CXXFLAGS) $(MOD_ALG_MIS_SRC) -o $(MOD_ALG_MIS_TARGET)

$(MOD_ALG_MASS_TARGET): $(MOD_ALG_MASS_SRC) $(MOD_ALG_HEADERS)
	$(CXX) $(CXXFLAGS) $(MOD_ALG_MASS_SRC) -o $(MOD_ALG_MASS_TARGET)

$(NO_SUP_MIS_TARGET): $(NO_SUP_MIS_SRC) $(NO_SUP_HEADERS)
	$(CXX) $(CXXFLAGS) $(NO_SUP_MIS_SRC) -o $(NO_SUP_MIS_TARGET)

$(NO_SUP_MASS_TARGET): $(NO_SUP_MASS_SRC) $(NO_SUP_HEADERS)
	$(CXX) $(CXXFLAGS) $(NO_SUP_MASS_SRC) -o $(NO_SUP_MASS_TARGET)

$(CHECK_OVERLAP_TARGET): $(CHECK_OVERLAP_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -I headers $(CHECK_OVERLAP_SRC) -o $(CHECK_OVERLAP_TARGET)

clean:
	rm -f $(ALL_TARGETS)

run_mis: $(MIS_TARGET)
	./$(MIS_TARGET) 5 5 v

run_mass: $(MASS_TARGET)
	./$(MASS_TARGET) 5 5 10

run_vis: $(VIS_TARGET)
	./$(VIS_TARGET) 5 5 100

run_mod_alg_mis: $(MOD_ALG_MIS_TARGET)
	./$(MOD_ALG_MIS_TARGET) 15 15 v

run_mod_alg_mass: $(MOD_ALG_MASS_TARGET)
	./$(MOD_ALG_MASS_TARGET) 5 5 10

run_mod_st_mis: $(MOD_ST_MIS_TARGET)
	./$(MOD_ST_MIS_TARGET) 5 5 v

run_mod_st_mass: $(MOD_ST_MASS_TARGET)
	./$(MOD_ST_MASS_TARGET) 5 5 10

run_no_sup_mis: $(NO_SUP_MIS_TARGET)
	./$(NO_SUP_MIS_TARGET) 5 5 v

run_no_sup_mass: $(NO_SUP_MASS_TARGET)
	./$(NO_SUP_MASS_TARGET) 5 5 10

.PHONY: all clean \
        run_mis run_mass run_vis \
        run_mod_alg_mis run_mod_alg_mass \
        run_mod_st_mis run_mod_st_mass \
        run_no_sup_mis run_no_sup_mass
