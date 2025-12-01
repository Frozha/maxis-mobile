#include "Simulator.hpp"
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

// Templated visualizer logger
template <typename Sim>
void run_visualizing_simulation(int rows, int cols, int max_steps = 1000) {
    // Calculate number of agents (half of total cells)
    int total_cells = rows * cols;
    int num_agents = total_cells / 2;

    // RNG setup
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Create all possible positions
    std::vector<Position> available_positions;
    available_positions.reserve(total_cells);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            available_positions.push_back({r, c});
        }
    }
    
    // Shuffle all positions
    std::shuffle(available_positions.begin(), available_positions.end(), gen);
    
    // Take first num_agents positions
    std::unordered_set<Position, PositionHash> occupied_positions;
    std::vector<Position> agent_positions;
    agent_positions.reserve(num_agents);
    
    for (int i = 0; i < num_agents && i < (int)available_positions.size(); ++i) {
        agent_positions.push_back(available_positions[i]);
        occupied_positions.insert(available_positions[i]);
    }
    
    // Create simulator (verbose = false so step() doesn't spam logs)
    auto sim = Sim(rows, cols, /*verbose=*/false);
    
    // Add all agents as GREEN
    for (const auto& pos : agent_positions) {
        sim.add_agent(pos, CellMask::GRE);
    }

    // First line: dimensions
    std::cout << rows << " " << cols << "\n";

    // Helper lambda to print ONLY the grid as 2D
    auto print_grid_2d = [&]() {
        const Grid& g = sim.grid();
        g.print_line();
        std::cout<<'\n';
    };

    // Print initial state (step 0)
    print_grid_2d();

    // Step loop: after each step, print grid again
    for (int step = 0; step < max_steps; ++step) {
        int actions = sim.step();  // single synchronous step
        print_grid_2d();
        if (actions == 0) {
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    // Choose simulator type here
    using SimType = Simulator;
    // using SimType = CollisionSimulator;

    int rows = 20;
    int cols = 20;
    int max_steps = 1000;

    if (argc >= 3) {
        rows = std::atoi(argv[1]);
        cols = std::atoi(argv[2]);
    } else {
        std::cout << "usage : ./simulate_visualizer_logger <rows> <cols> [max_steps]\n";
        return -1;
    }

    if (argc >= 4) {
        max_steps = std::atoi(argv[3]);
    }

    run_visualizing_simulation<SimType>(rows, cols, max_steps);
    return 0;
}
