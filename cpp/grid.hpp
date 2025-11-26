#pragma once
#include "mask.hpp"
#include <vector>
#include <optional>
#include<iostream>

struct Position {
    int row;
    int col;
    
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

class Grid {
private:
    int rows_;
    int cols_;
    std::vector<CellMask> cells_;  // Flattened 2D grid
    
    // Helper to convert 2D position to 1D index
    int index(int row, int col) const {
        return row * cols_ + col;
    }
    
    int index(const Position& pos) const {
        return index(pos.row, pos.col);
    }
    
    // Check if position is within bounds
    bool in_bounds(int row, int col) const {
        return row >= 0 && row < rows_ && col >= 0 && col < cols_;
    }
    
    bool in_bounds(const Position& pos) const {
        return in_bounds(pos.row, pos.col);
    }

public:
    Grid(int rows, int cols) 
        : rows_(rows), cols_(cols), cells_(rows * cols, CellMask::EMP) {
        //nothing needed empty grid initialized
    }
    
    // Get cell at position
    CellMask get(const Position& pos) const {
        if (!in_bounds(pos)) return CellMask::BOU;
        return cells_[index(pos)];
    }
    
    // Set cell at position
    void set(const Position& pos, CellMask value) {
        if (in_bounds(pos)) {
            cells_[index(pos)] = value;
        }
    }
    
    // Generate view for an agent at given position
    // View layout (13 cells in cross pattern):
    //       U2
    //     UL U1 UR
    //   L2 L1 C R1 R2
    //     DL D1 DR
    //       D2
    View get_view(const Position& pos) const {
        View view{};
        
        // Center
        view[int(ViewIndex::C)] = get(pos);
        
        // Up direction
        view[int(ViewIndex::U1)] = get({pos.row - 1, pos.col});
        view[int(ViewIndex::U2)] = get({pos.row - 2, pos.col});
        
        // Down direction
        view[int(ViewIndex::D1)] = get({pos.row + 1, pos.col});
        view[int(ViewIndex::D2)] = get({pos.row + 2, pos.col});
        
        // Left direction
        view[int(ViewIndex::L1)] = get({pos.row, pos.col - 1});
        view[int(ViewIndex::L2)] = get({pos.row, pos.col - 2});
        
        // Right direction
        view[int(ViewIndex::R1)] = get({pos.row, pos.col + 1});
        view[int(ViewIndex::R2)] = get({pos.row, pos.col + 2});
        
        // Diagonals
        view[int(ViewIndex::UL)] = get({pos.row - 1, pos.col - 1});
        view[int(ViewIndex::UR)] = get({pos.row - 1, pos.col + 1});
        view[int(ViewIndex::DL)] = get({pos.row + 1, pos.col - 1});
        view[int(ViewIndex::DR)] = get({pos.row + 1, pos.col + 1});
        
        return view;
    }
        
    // Get dimensions
    int rows() const { return rows_; }
    int cols() const { return cols_; }
    
    // Clear a position (set to empty)
    void clear(const Position& pos) {
        set(pos, CellMask::EMP);
    }
    
    // Try to move an agent - returns true if successful
    bool try_move(const Position& from, const Position& to, CellMask color) {
        clear(from);
        set(to, color);
        return true;
    }
    
    // Print grid for debugging (optional)
    void print() const {
        for (int r = 0; r < rows_; ++r) {
            for (int c = 0; c < cols_; ++c) {
                CellMask cell = cells_[index(r, c)];
                char ch = '?';
                if (cell == CellMask::BOU) ch = 'x';
                else if (cell == CellMask::EMP) ch = '.';
                else if (cell == CellMask::RED) ch = 'r';
                else if (cell == CellMask::BLU) ch = 'b';
                else if (cell == CellMask::GRE) ch = 'g';
                std::cout << ch;
            }
            std::cout << '\n';
        }
    }
};