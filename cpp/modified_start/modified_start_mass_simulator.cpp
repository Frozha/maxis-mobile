#include "../headers/simulator.hpp"
#include <iostream>
#include <random>
#include <unordered_set>
#include <algorithm>

// Helper struct for hashing Position objects
struct PositionHash {
    std::size_t operator()(const Position& p) const {
        return std::hash<int>()(p.row) ^ (std::hash<int>()(p.col) << 1);
    }
};

// Random number generation setup
std::random_device rd;
std::mt19937 gen(rd());

template <typename Sim>
int run_simulation(int rows, int cols, bool verbose = false, bool log_verbose=true, int max_steps=10000) {
    // Calculate number of agents (half of total cells)
    int total_cells = rows * cols;
    int num_agents = (total_cells + 1) / 2;

    
    // Create all possible positions
    std::vector<Position> available_positions;
    available_positions.reserve(total_cells);
    for (int r = 1; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            available_positions.push_back({r, c});
        }
    }
    
    // Shuffle all positions
    std::shuffle(available_positions.begin(), available_positions.end(), gen);
    
    // Take the first num_agents positions
    std::unordered_set<Position, PositionHash> occupied_positions;
    std::vector<Position> agent_positions;
    agent_positions.reserve(num_agents);
    
    for (int i = 0; i < num_agents && i < (int)available_positions.size(); ++i) {
        agent_positions.push_back(available_positions[i]);
        occupied_positions.insert(available_positions[i]);
    }
    
    // Use template param as simulator type (auto for value)
    auto sim = Sim(rows, cols, verbose);
    
    // Add all agents at generated positions (all starting as GREEN)
    for (const auto& pos : agent_positions) {
        sim.add_agent(pos, CellMask::GRE);
    }
    
    // Initial state (single-line)
    if(log_verbose) {sim.print_state_line();}

    // Run simulation
    sim.run(max_steps);
    
    // Final raw stats (your encoded format)
    if(log_verbose){sim.raw_final_r_g_b_stats();}
    return sim.success();
}

int main(int argc, char* argv[]) {
     using SimType = Simulator;

    
    int rows = 20;
    int cols = 20;
    bool verbose = false;
    bool log_verbose = true;
    int n = 100;
    int max_steps = 10000;

    // Parse command line arguments if provided
    if (argc >= 4) {
        rows = std::atoi(argv[1]);
        cols = std::atoi(argv[2]);
        n = std::atoi(argv[3]);
    }
    else {
        std::cout << "usage : ./mod_start_mass_simulate <rows> <cols> <n_simulations> [max_steps] [f=disable_log]\n";
        return -1;
    }
    if (argc >= 5) {
        max_steps = std::atoi(argv[4]);
    }
    if (argc >= 6) {
        log_verbose = false;
    }
    if(log_verbose){
    std::cout << rows << " " << cols << "\n";
    std::cout << n << "\n";
    }
    long long success_count = 0;

    while (n--) {
        success_count+=run_simulation<SimType>(rows, cols, verbose, log_verbose, max_steps);
        if(log_verbose) std::cout << "\n";
    }
    std::cerr<<"success : "<<success_count<<"\n";

    return 0;
}
