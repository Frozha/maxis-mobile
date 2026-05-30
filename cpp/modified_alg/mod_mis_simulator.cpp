#include "CollisionSimulator.hpp" //local new simulator
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

//  plug in either Simulator or CollisionSimulator
template <typename Sim>
void run_simulation(int rows, int cols, bool verbose = false) {
    std::cout << "=== MIS Simulation with Collision Avoidance: "
              << rows << "x" << cols << " Grid ===\n\n";
    
    // Calculate number of agents (half of total cells)
    int total_cells = rows * cols;
    int num_agents = (total_cells + 1) / 2;
    
    std::cout << "Grid size: " << rows << " x " << cols << "\n";
    std::cout << "Total cells: " << total_cells << "\n";
    std::cout << "Number of agents: " << num_agents << "\n\n";
    

    
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
    
    // Take the first num_agents positions
    std::unordered_set<Position, PositionHash> occupied_positions;
    std::vector<Position> agent_positions;
    agent_positions.reserve(num_agents);
    
    for (int i = 0; i < num_agents && i < (int)available_positions.size(); ++i) {
        agent_positions.push_back(available_positions[i]);
        occupied_positions.insert(available_positions[i]);
    }
    
    std::cout << "Generated " << agent_positions.size() << " random positions\n";
    std::cout << "All agents start as GREEN\n\n";
    
    auto sim = Sim(rows, cols, verbose);
    
    // Add all agents at generated positions (all starting as GREEN)
    for (const auto& pos : agent_positions) {
        sim.add_agent(pos, CellMask::GRE);
    }
    
    std::cout << "Initial state:\n";
    sim.print_state();
    std::cout << "\n";
    
    // Run simulation
    sim.run(10000);  // Max 1000 steps

    std::cout << "\nFinal state:\n";
    sim.print_state();
    std::cout << "\nFinal statistics:\n\n";
    
    sim.print_statistics();
}

int main(int argc, char* argv[]) {
    // Default grid size
    int rows = 20;
    int cols = 20;
    bool verbose = true;
    
    // Parse command line arguments if provided
    if (argc >= 3) {
        rows = std::atoi(argv[1]);
        cols = std::atoi(argv[2]);
    }
    if (argc >= 4) {
        verbose = (std::string(argv[3]) == "v" || std::string(argv[3]) == "verbose");
    }
    
    // Validate grid size
    if (rows < 4 || cols < 4) {
        std::cerr << "Error: Grid must be at least 4x4\n";
        return 1;
    }
    
    if (rows > 100 || cols > 100) {
        std::cerr << "Warning: Large grid size may take a long time\n";
    }
    

    run_simulation<CollisionSimulator>(rows, cols, verbose);
    //run_simulation<Simulator>(rows, cols, verbose);
    std::cout << "\nUsage: " << argv[0] << " [rows] [cols] [verbose(default)]\n";
    std::cout << "Example: " << argv[0] << " 15 15\n";
    
    return 0;
}
