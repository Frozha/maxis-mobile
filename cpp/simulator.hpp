#pragma once
#include "grid.hpp"
#include "agent.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>

struct MoveRequest {
    int agent_id;
    Position target;
    CellMask new_color;  // Color after all changes
};

class Simulator {
private:
    Grid grid_;
    std::vector<Agent> agents_;
    int step_count_;
    bool verbose_;
    
    
public:
    Simulator(int rows, int cols, bool verbose = false) 
        : grid_(rows, cols), step_count_(0), verbose_(verbose) {}
    
    // Add an agent to the simulation
    void add_agent(const Position& pos, CellMask color) {
        int id = agents_.size();
        agents_.emplace_back(id, pos, color);
        grid_.set(pos, color);
    }
    
    // Run one step of the simulation
    // Returns the number of agents that performed actions
    int step() {
        step_count_++;
        
        if (verbose_) {
            std::cout << "\n=== Step " << step_count_ << " ===\n";
        }
        
        // Phase 1: All agents observe and decide
        std::vector<std::pair<int, Action>> decisions;
        decisions.reserve(agents_.size());
        
        for (const auto& agent : agents_) {
            View view = grid_.get_view(agent.position());
            Action action = agent.decide(view);
            decisions.push_back({agent.id(), action});
            
            if (verbose_ && action != Action::NONE) {
                std::cout << "Agent " << agent.id() 
                         << " at (" << agent.position().row 
                         << "," << agent.position().col 
                         << ") decides: " << int(action) << "\n";
            }
        }
        
        // Phase 2: Process moves - collect all move requests
        std::vector<MoveRequest> move_requests;
        
        for (size_t i = 0; i < agents_.size(); ++i) {
            Agent& agent = agents_[i];
            Action action = decisions[i].second;
            
            if (action == Action::NONE) continue;
            
            // Compute final position and color
            Position target = agent.position();
            auto move_target = agent.compute_target(action);
            if (move_target.has_value()) {
                target = move_target.value();
            }
            
            CellMask new_color = agent.color();
            auto color_change = agent.compute_new_color(action);
            if (color_change.has_value()) {
                new_color = color_change.value();
            }
            
            move_requests.push_back({agent.id(), target, new_color});
        }
        
        // Phase 3: Validate and apply moves
        int successful_actions = 0;
        
        for (const auto& request : move_requests) {
            Agent& agent = agents_[request.agent_id];
            Position old_pos = agent.position();
            Position new_pos = request.target;
            CellMask new_color = request.new_color;
            
            // Check for conflicts
            if (new_pos != old_pos) {
                // Agent wants to move
                // Move is valid
                grid_.clear(old_pos);
                grid_.set(new_pos, new_color);
                agent.update_position(new_pos);
                agent.update_color(new_color);
                successful_actions++;
                
                if (verbose_) {
                    std::cout << "Agent " << agent.id() 
                             << " moved to (" << new_pos.row 
                             << "," << new_pos.col << ")\n";
                }
            } else {
                // Agent only changes color (no move)
                if (new_color != agent.color()) {
                    grid_.set(old_pos, new_color);
                    agent.update_color(new_color);
                    successful_actions++;
                    
                    if (verbose_) {
                        std::cout << "Agent " << agent.id() 
                                 << " changed color\n";
                    }
                }
            }
        }
        
        return successful_actions;
    }
    
    // Run simulation until convergence or max steps
    void run(int max_steps = 1000) {
        std::cout << "Starting simulation...\n";
        print_state();
        
        for (int i = 0; i < max_steps; ++i) {
            int actions = step();
            
            if (verbose_) {
                print_state();
            }
            
            
            if (actions == 0) {
                std::cout << "\nConverged after " << step_count_ 
                         << " steps (no more actions)\n";
                break;
            }
        }
        
        std::cout << "\nFinal state:\n";
        print_state();
    }
    
    // Print current state
    void print_state() const {
        grid_.print();
        std::cout << "Agents: " << agents_.size() << "\n";
    }
    
    // Get current step count
    int steps() const { return step_count_; }
    
    // Count agents by color
    void print_statistics() const {
        int red = 0, blue = 0, green = 0;
        for (const auto& agent : agents_) {
            if (agent.color() == CellMask::RED) red++;
            else if (agent.color() == CellMask::BLU) blue++;
            else if (agent.color() == CellMask::GRE) green++;
        }
        std::cout << "Statistics - Red: " << red 
                 << ", Blue: " << blue 
                 << ", Green: " << green << "\n";
    }
    
    // Access grid (for testing/visualization)
    const Grid& grid() const { return grid_; }
    
    // Access agents (for testing/analysis)
    const std::vector<Agent>& agents() const { return agents_; }
};