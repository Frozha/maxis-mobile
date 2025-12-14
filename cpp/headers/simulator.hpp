#pragma once
#include "grid.hpp"
#include "agent.hpp"
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <iostream>
#include<algorithm>

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
        if (verbose_) {
        std::cout << "Starting simulation...\n";
        print_state();}
        
        for (int i = 0; i < max_steps; ++i) {
            int actions = step();
            
            if (verbose_) {
                print_state();
            }
            
            
            if (actions == 0) {
                if (verbose_) {
                std::cout << "\nConverged after " << step_count_ 
                         << " steps (no more actions)\n";}
                break;
            }
        }
    }
    
    // Print current state
    void print_state() const {
        grid_.print();
        std::cout << "Agents: " << agents_.size() << "\n";
    }
    
    void print_state_line() const {
        grid_.print_line();
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
    
    int success() const{
        int red = 0, blue = 0, green = 0;
        for (const auto& agent : agents_) {
            if (agent.color() == CellMask::RED) red++;
            else if (agent.color() == CellMask::BLU) blue++;
            else if (agent.color() == CellMask::GRE) green++;
        }
        return ((green==0)&&(blue==0))?1:0;
    }

    void raw_final_r_g_b_stats() const {
        grid_.print_line();
        int red = 0, blue = 0, green = 0;
        for (const auto& agent : agents_) {
            if (agent.color() == CellMask::RED) red++;
            else if (agent.color() == CellMask::BLU) blue++;
            else if (agent.color() == CellMask::GRE) green++;
        }
        std::cout<<red<<", "<< green << ", "<<blue<<", "<<step_count_<<", "<<((green==0)&&(blue==0))?1:0;
    }
    // Access grid (for testing/visualization)
    const Grid& grid() const { return grid_; }
    
    // Access agents (for testing/analysis)
    const std::vector<Agent>& agents() const { return agents_; }

    
};


/*
// New simulator with collision prevention
class CollisionSimulator {
private:
    Grid grid_;
    std::vector<Agent> agents_;
    int step_count_;
    bool verbose_;
    
    struct MoveRequestEx {
        int agent_id;
        Position old_pos;
        Position target;
        CellMask new_color;
        Action action;
    };

public:
    CollisionSimulator(int rows, int cols, bool verbose = false) 
        : grid_(rows, cols), step_count_(0), verbose_(verbose) {}
    
    // Add an agent to the simulation
    void add_agent(const Position& pos, CellMask color) {
        int id = (int)agents_.size();
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
            //Action action = agent.custom_decide(view);
            decisions.push_back({agent.id(), action});
            
            if (verbose_ && action != Action::NONE) {
                std::cout << "Agent " << agent.id() 
                          << " at (" << agent.position().row 
                          << "," << agent.position().col 
                          << ") decides: " << int(action) << "\n";
            }
        }
        
        // Phase 2: Build move/color requests
        std::vector<MoveRequestEx> move_requests;
        move_requests.reserve(agents_.size());
        
        for (size_t i = 0; i < agents_.size(); ++i) {
            Agent& agent = agents_[i];
            Action action = decisions[i].second;
            
            if (action == Action::NONE) continue;
            
            Position old_pos = agent.position();
            Position target = old_pos;
            
            auto move_target = agent.compute_target(action);
            if (move_target.has_value()) {
                target = move_target.value();
            }
            
            CellMask new_color = agent.color();
            auto color_change = agent.compute_new_color(action);
            if (color_change.has_value()) {
                new_color = color_change.value();
            }
            
            move_requests.push_back(
                MoveRequestEx{agent.id(), old_pos, target, new_color, action}
            );
        }
        
        // Phase 3: Collision handling and applying moves
        
        // Occupancy at start of step
        using PosKey = std::pair<int,int>;
        std::map<PosKey, int> initial_occupancy; // (row,col) -> agent_id
        for (const auto& agent : agents_) {
            initial_occupancy[{agent.position().row, agent.position().col}] = agent.id();
        }
        
        // Map target positions to agents that want to MOVE there (target != old_pos)
        std::map<PosKey, std::vector<int>> target_to_agents;
        std::unordered_map<int, size_t> agent_to_req_index; // for moving agents
        
        for (size_t i = 0; i < move_requests.size(); ++i) {
            const auto& req = move_requests[i];
            if (req.target.row != req.old_pos.row || req.target.col != req.old_pos.col) {
                PosKey key{req.target.row, req.target.col};
                target_to_agents[key].push_back(req.agent_id);
                agent_to_req_index[req.agent_id] = i;
            }
        }
        
        std::set<int> winners_moving;   // agents allowed to move
        std::set<int> blocked_moving;   // agents whose moves are blocked
        
        // Resolve collisions per target cell
        for (auto& entry : target_to_agents) {
            const PosKey& key = entry.first;
            auto& agent_list = entry.second;
            
            // Is there a stationary occupant in this cell?
            bool has_stationary = false;
            int stationary_id = -1;
            
            auto occ_it = initial_occupancy.find(key);
            if (occ_it != initial_occupancy.end()) {
                int occ_id = occ_it->second;
                // If this occupant is NOT in agent_to_req_index, it is not moving
                if (agent_to_req_index.find(occ_id) == agent_to_req_index.end()) {
                    has_stationary = true;
                    stationary_id = occ_id;
                }
            }
            
            if (has_stationary) {
                // Stationary occupant wins, all movers to this cell are blocked
                for (int mover_id : agent_list) {
                    blocked_moving.insert(mover_id);
                    if (verbose_) {
                        const auto& req_mover = move_requests[agent_to_req_index[mover_id]];
                        std::cout << "Agent " << mover_id
                                  << " was restricted to move " << int(req_mover.action)
                                  << " due to agent " << stationary_id
                                  << " moving " << int(Action::NONE) << "\n";
                    }
                }
            } else if (agent_list.size() == 1) {
                // Single mover to an empty or vacated cell: allowed
                winners_moving.insert(agent_list[0]);
            } else {
                // Multiple movers to same target: pick smallest id as winner
                int winner = *std::min_element(agent_list.begin(), agent_list.end());
                winners_moving.insert(winner);
                
                const auto& req_winner = move_requests[agent_to_req_index[winner]];
                
                for (int mover_id : agent_list) {
                    if (mover_id == winner) continue;
                    blocked_moving.insert(mover_id);
                    if (verbose_) {
                        const auto& req_mover = move_requests[agent_to_req_index[mover_id]];
                        std::cout << "Agent " << mover_id
                                  << " was restricted to move " << int(req_mover.action)
                                  << " due to agent " << winner
                                  << " moving " << int(req_winner.action) << "\n";
                    }
                }
            }
        }
        
        // Apply moves and color changes
        int successful_actions = 0;
        
        // First, clear all cells that will be vacated by successful movers
        for (const auto& req : move_requests) {
            if (req.target.row != req.old_pos.row || req.target.col != req.old_pos.col) {
                // It's a move request
                if (winners_moving.count(req.agent_id)) {
                    grid_.clear(req.old_pos);
                }
            }
        }
        
        // Now apply moves and color changes
        for (const auto& req : move_requests) {
            Agent& agent = agents_[req.agent_id];
            
            bool is_move = (req.target.row != req.old_pos.row || req.target.col != req.old_pos.col);
            
            if (is_move) {
                if (winners_moving.count(req.agent_id)) {
                    // Move is allowed
                    grid_.set(req.target, req.new_color);
                    agent.update_position(req.target);
                    agent.update_color(req.new_color);
                    successful_actions++;
                    
                    if (verbose_) {
                        std::cout << "Agent " << agent.id() 
                                  << " moved to (" << req.target.row 
                                  << "," << req.target.col << ")\n";
                    }
                }
                // else: move was blocked, do nothing (no color change)
            } else {
                // Only color change (no move)
                if (req.new_color != agent.color()) {
                    grid_.set(req.old_pos, req.new_color);
                    agent.update_color(req.new_color);
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
        if (verbose_) {
            std::cout << "Starting simulation...\n";
            print_state();
        }
        
        for (int i = 0; i < max_steps; ++i) {
            int actions = step();
            
            if (verbose_) {
                print_state();
            }
            
            if (actions == 0) {
                if (verbose_) {
                    std::cout << "\nConverged after " << step_count_ 
                              << " steps (no more actions)\n";
                }
                break;
            }
        }
    }
    
    // Print current state
    void print_state() const {
        grid_.print();
        std::cout << "Agents: " << agents_.size() << "\n";
    }
    
    void print_state_line() const {
        grid_.print_line();
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
    
    void raw_final_r_g_b_stats() const {
        grid_.print_line();
        int red = 0, blue = 0, green = 0;
        for (const auto& agent : agents_) {
            if (agent.color() == CellMask::RED) red++;
            else if (agent.color() == CellMask::BLU) blue++;
            else if (agent.color() == CellMask::GRE) green++;
        }
        std::cout << red << ", " << green << ", " << blue
                  << ", " << step_count_ << ", "
                  << (((green == 0) && (blue == 0)) ? 1 : 0);
    }
    
    // Access grid (for testing/visualization)
    const Grid& grid() const { return grid_; }
    
    // Access agents (for testing/analysis)
    const std::vector<Agent>& agents() const { return agents_; }
};
*/