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
int run_simulation(int rows, int cols, bool verbose = false, bool log_verbose = true) {
    int total_cells = rows * cols;
    int num_agents = std::ceil((float)total_cells / 2);

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

    auto sim = Sim(rows, cols, verbose);
    for (const auto& pos : agent_positions)
        sim.add_agent(pos, CellMask::GRE);

    if (log_verbose) sim.print_state_line();

    sim.run(10000);

    if (log_verbose) sim.raw_final_r_g_b_stats();
    return sim.success();
}

int main(int argc, char* argv[]) {
    int rows = 20, cols = 20;
    bool verbose = false, log_verbose = true;
    int n = 100;

    if (argc >= 4) {
        rows = std::atoi(argv[1]);
        cols = std::atoi(argv[2]);
        n    = std::atoi(argv[3]);
    } else {
        std::cout << "usage: ./no_suppression_mass <rows> <cols> <n_simulations> [any=disable_log]\n";
        return -1;
    }
    if (argc >= 5) log_verbose = false;

    if (log_verbose) {
        std::cout << rows << " " << cols << "\n";
        std::cout << n << "\n";
    }

    long long success_count = 0;
    while (n--) {
        success_count += run_simulation<NoSuppressionSimulator>(rows, cols, verbose, log_verbose);
        if (log_verbose) std::cout << "\n";
    }
    std::cerr << "success : " << success_count << "\n";
    return 0;
}
