#pragma once
#include "mask.hpp"
#include "grid.hpp"
#include <optional>

class Agent {
private:
    Position position_;
    CellMask color_;
    int id_;  // For debugging/tracking
    
public:
    Agent(int id, Position pos, CellMask color) 
        : id_(id), position_(pos), color_(color) {
        // Ensure color is one of the robot types
        if (color != CellMask::RED && 
            color != CellMask::BLU && 
            color != CellMask::GRE) {
            throw std::invalid_argument("Agent must be RED, BLUE, or GREEN");
        }
    }
    
    // Get current position
    Position position() const { return position_; }
    
    // Get current color
    CellMask color() const { return color_; }
    
    // Get ID
    int id() const { return id_; }
    
    // Update position (called by simulator after successful move)
    void update_position(const Position& new_pos) {
        position_ = new_pos;
    }
    
    // Update color (called by simulator after color change)
    void update_color(CellMask new_color) {
        color_ = new_color;
    }
    
    // Decide action based on current view
    Action decide(const View& view) const {
        switch (color_) {
            case CellMask::GRE:
                return decide_green(view);
            case CellMask::BLU:
                return decide_blue(view);
            case CellMask::RED:
                // Red robots don't have rules in the provided code
                return Action::NONE;
            default:
                return Action::NONE;
        }
    }
    
    Action suppression_aware_decide(const View& view) const {
        switch (color_) {
            case CellMask::GRE: return suppression_aware_decide_green(view);
            case CellMask::BLU: return decide_blue(view);
            case CellMask::RED: return Action::NONE;
            default:            return Action::NONE;
        }
    }

    Action custom_decide(const View& view) const {
        Action decision = Action::NONE;

        switch (color_) {
            case CellMask::GRE:
                decision =  decide_green(view);
                break;
            case CellMask::BLU:
                decision = decide_blue(view);
                break;
            case CellMask::RED:
                // Red robots don't have rules in the provided code
                return Action::NONE;
        }
        if(decision == Action::NONE) decision = decide_custom_rules(view);
        return decision;
    }

    // Compute the target position for a move action
    std::optional<Position> compute_target(Action action) const {
        if (has(action, Action::MOVE_UP)) {
            return Position{position_.row - 1, position_.col};
        }
        if (has(action, Action::MOVE_DOWN)) {
            return Position{position_.row + 1, position_.col};
        }
        if (has(action, Action::MOVE_LEFT)) {
            return Position{position_.row, position_.col - 1};
        }
        if (has(action, Action::MOVE_RIGHT)) {
            return Position{position_.row, position_.col + 1};
        }
        return std::nullopt;  // No movement
    }
    
    // Extract color change from action
    std::optional<CellMask> compute_new_color(Action action) const {
        if (has(action, Action::BECOME_BLUE)) {
            return CellMask::BLU;
        }
        if (has(action, Action::BECOME_RED)) {
            return CellMask::RED;
        }
        if (has(action, Action::BECOME_GREEN)) {
            return CellMask::GRE;
        }
        return std::nullopt;  // No color change
    }
};