#include "NoSuppressionSimulator.hpp"
#include <iostream>
#include <random>
#include <unordered_set>
#include <algorithm>
#include <math.h>

struct PositionHash {
    std::size_t operator()(const Position& p) const {
        return std::hash<int>()(p.row) ^ (std::hash<int>()(p.col) << 1);
    }
};

std::random_device rd;
std::mt19937 gen(rd());

template <typename Sim>
void run_simulation(int rows, int cols, bool verbose = false) {
    std::cout << "=== MIS Simulation (no-suppression): "
              << rows << "x" << cols << " Grid ===\n\n";

    int total_cells = rows * cols;
    int num_agents = std::ceil((float)total_cells / 2);

    std::cout << "Grid size: " << rows << " x " << cols << "\n";
    std::cout << "Total cells: " << total_cells << "\n";
    std::cout << "Number of agents: " << num_agents << "\n\n";

    std::vector<Position> available_positions;
    available_positions.reserve(total_cells);
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            available_positions.push_back({r, c});

    std::shuffle(available_positions.begin(), available_positions.end(), gen);

    std::vector<Position> agent_positions;
    agent_positions.reserve(num_agents);
    for (int i = 0; i < num_agents && i < (int)available_positions.size(); ++i)
        agent_positions.push_back(available_positions[i]);

    std::cout << "Generated " << agent_positions.size() << " random positions\n";
    std::cout << "All agents start as GREEN\n\n";

    auto sim = Sim(rows, cols, verbose);
    for (const auto& pos : agent_positions)
        sim.add_agent(pos, CellMask::GRE);

    std::cout << "Initial state:\n";
    sim.print_state();
    std::cout << "\n";

    sim.run(10000);

    std::cout << "\nFinal state:\n";
    sim.print_state();
    std::cout << "\nSuccess: " << sim.success() << "\n";
}

int main(int argc, char* argv[]) {
    int rows = 20, cols = 20;
    bool verbose = true;

    if (argc >= 3) {
        rows = std::atoi(argv[1]);
        cols = std::atoi(argv[2]);
    }
    if (argc >= 4)
        verbose = (std::string(argv[3]) == "v" || std::string(argv[3]) == "verbose");

    if (rows < 4 || cols < 4) {
        std::cerr << "Error: Grid must be at least 4x4\n";
        return 1;
    }

    run_simulation<NoSuppressionSimulator>(rows, cols, verbose);
    return 0;
}
