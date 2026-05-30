#pragma once
#include "../headers/grid.hpp"
#include "../headers/agent.hpp"
#include <vector>
#include <iostream>

// Identical to the base Simulator except step() calls
// agent.suppression_aware_decide() instead of agent.decide().
// This isolates the effect of priority reordering only —
// no collision handling, no custom rules.
class NoSuppressionSimulator {
private:
    Grid grid_;
    std::vector<Agent> agents_;
    int step_count_;
    bool verbose_;

public:
    NoSuppressionSimulator(int rows, int cols, bool verbose = false)
        : grid_(rows, cols), step_count_(0), verbose_(verbose) {}

    void add_agent(const Position& pos, CellMask color) {
        int id = agents_.size();
        agents_.emplace_back(id, pos, color);
        grid_.set(pos, color);
    }

    int step() {
        step_count_++;

        if (verbose_)
            std::cout << "\n=== Step " << step_count_ << " ===\n";

        // Phase 1: all agents observe and decide using suppression-aware priority
        std::vector<std::pair<int, Action>> decisions;
        decisions.reserve(agents_.size());

        for (const auto& agent : agents_) {
            View view = grid_.get_view(agent.position());
            Action action = agent.suppression_aware_decide(view);
            decisions.push_back({agent.id(), action});

            if (verbose_ && action != Action::NONE)
                std::cout << "Agent " << agent.id()
                          << " at (" << agent.position().row
                          << "," << agent.position().col
                          << ") decides: " << int(action) << "\n";
        }

        // Phase 2: collect move requests
        struct MoveRequest { int agent_id; Position target; CellMask new_color; };
        std::vector<MoveRequest> move_requests;

        for (size_t i = 0; i < agents_.size(); ++i) {
            Agent& agent = agents_[i];
            Action action = decisions[i].second;
            if (action == Action::NONE) continue;

            Position target = agent.position();
            auto move_target = agent.compute_target(action);
            if (move_target.has_value()) target = move_target.value();

            CellMask new_color = agent.color();
            auto color_change = agent.compute_new_color(action);
            if (color_change.has_value()) new_color = color_change.value();

            move_requests.push_back({agent.id(), target, new_color});
        }

        // Phase 3: apply moves (no collision handling — mirrors base Simulator)
        int successful_actions = 0;

        for (const auto& request : move_requests) {
            Agent& agent = agents_[request.agent_id];
            Position old_pos = agent.position();
            Position new_pos = request.target;
            CellMask new_color = request.new_color;

            if (new_pos != old_pos) {
                grid_.clear(old_pos);
                grid_.set(new_pos, new_color);
                agent.update_position(new_pos);
                agent.update_color(new_color);
                successful_actions++;

                if (verbose_)
                    std::cout << "Agent " << agent.id()
                              << " moved to (" << new_pos.row
                              << "," << new_pos.col << ")\n";
            } else {
                if (new_color != agent.color()) {
                    grid_.set(old_pos, new_color);
                    agent.update_color(new_color);
                    successful_actions++;

                    if (verbose_)
                        std::cout << "Agent " << agent.id() << " changed color\n";
                }
            }
        }

        return successful_actions;
    }

    void run(int max_steps = 10000) {
        if (verbose_) { std::cout << "Starting simulation...\n"; print_state(); }

        for (int i = 0; i < max_steps; ++i) {
            int actions = step();
            if (verbose_) print_state();
            if (actions == 0) {
                if (verbose_)
                    std::cout << "\nConverged after " << step_count_ << " steps\n";
                break;
            }
        }
    }

    void print_state() const { grid_.print(); std::cout << "Agents: " << agents_.size() << "\n"; }
    void print_state_line() const { grid_.print_line(); }
    int steps() const { return step_count_; }
    const Grid& grid() const { return grid_; }
    const std::vector<Agent>& agents() const { return agents_; }

    int success() const {
        for (const auto& a : agents_)
            if (a.color() == CellMask::GRE || a.color() == CellMask::BLU)
                return 0;
        return 1;
    }

    void raw_final_r_g_b_stats() const {
        grid_.print_line();
        int red = 0, blue = 0, green = 0;
        for (const auto& a : agents_) {
            if      (a.color() == CellMask::RED) red++;
            else if (a.color() == CellMask::BLU) blue++;
            else if (a.color() == CellMask::GRE) green++;
        }
        std::cout << red << ", " << green << ", " << blue
                  << ", " << step_count_ << ", "
                  << (((green == 0) && (blue == 0)) ? 1 : 0);
    }
};
