# Maximal Independent Set Solver - Grid-Based Agent Simulation

A distributed algorithm implementation for finding the maximal independent set in grid graphs using autonomous agents with local decision rules.

## Architecture

The implementation is split into clean, maintainable components:

### Core Files

1. **mask.hpp** 
   - Defines cell types (boundary, empty, red, blue, green robots)
   - Pattern matching logic for agent decision rules
   - Action definitions (move, color change)
   - All 66 pattern rules (G1-G6, B1-B4) for green and blue robots

2. **grid.hpp**
   - Manages the 2D grid state
   - Provides local views (13-cell cross pattern) for agents
   - Handles boundary conditions
   - No decision-making logic (passive data structure)

3. **agent.hpp**
   - Represents autonomous agents (robots)
   - Stores position and color state
   - Makes decisions based on local views using pattern rules
   - Computes target positions and color changes

4. **simulator.hpp**
   - Orchestrates the simulation loop
   - Coordinates agent actions (observe → decide → act)
   - Handles collision detection and conflict resolution (not implemented yet)
   - Provides convergence detection

5. **main.cpp**
   - Example usage

## Design Decisions

### Why Keep the Global Grid?

After analysis, we chose a centralized grid approach because:

1. **Synchronous Algorithm**: The MIS algorithm operates in rounds (all agents observe, decide, then act simultaneously)
2. **Spatial Structure**: The grid IS the graph we're solving - it's the natural representation
3. **View Generation**: Each agent needs a 13-cell neighborhood, which requires querying adjacent cells
4. **Collision Prevention**: Multiple agents can't occupy the same cell - centralized validation is simplest (not implemented)
5. **Visualization**: Debugging and analysis require collecting global state anyway

### Architecture Benefits

- **Separation of Concerns**: Grid (storage) ↔ Agent (logic) ↔ Simulator (coordination)
- **Agent Autonomy**: Agents make independent decisions based only on local views
- **Maintainability**: Each component has a single, clear responsibility
- **Testability**: Can test agents, grid, and simulation independently

## Building

```bash
make           # Build the simulator
make run       # Build and run
make clean     # Remove build artifacts
```

## Usage Example

```cpp
// Create a 10x10 grid
Simulator sim(10, 10, verbose=true);

// Add agents at positions with colors
sim.add_agent({3, 3}, CellMask::GRE);  // Green at (3,3)
sim.add_agent({3, 5}, CellMask::RED);  // Red at (3,5)
sim.add_agent({5, 3}, CellMask::BLU);  // Blue at (5,3)

// Run simulation (max 50 steps)
sim.run(50);

// Print final statistics
sim.print_statistics();
```

## How It Works

### Simulation Cycle

Each simulation step follows this pattern:

1. **Observe Phase**: Each agent queries its local view from the grid
2. **Decide Phase**: Agents independently apply pattern-matching rules to decide actions
3. **Act Phase**: Simulator validates and applies actions (handling conflicts)

### View Structure

Each agent sees a 13-cell cross pattern:
```
      U2
    UL U1 UR
  L2 L1 C R1 R2
    DL D1 DR
      D2
```

Where:
- `C` = center (agent's position)
- `U1, U2` = up (1 and 2 cells)
- `D1, D2` = down
- `L1, L2` = left
- `R1, R2` = right
- `UL, UR, DL, DR` = diagonals

### Conflict Resolution

- **Mo Conflict Algorithm**: Algorithm is designed so that no conflict present
- **Simultaneous Actions**: All agents act in the same timestep (synchronous)
- **Future** : might need to implement if appending new views in the algorithm
### Convergence

The simulation stops when:
- No agent performs any action (stable state reached)
- Maximum step count is reached

## Pattern Rules

The implementation includes all rules from the paper:

- **G1**: Green moves left (4 patterns)
- **G2**: Green moves down (4 patterns)
- **G3**: Green moves right (7 patterns)
- **G4**: Green moves up (20 patterns)
- **G5**: Green becomes blue (14 patterns)
- **G6**: Green becomes red (7 patterns)
- **B1**: Blue becomes green and moves right (3 patterns)
- **B2**: Blue becomes green and moves left (1 pattern)
- **B3**: Blue becomes green and moves down (1 pattern)
- **B4**: Blue becomes green (5 patterns)

Red robots have no active rules in this implementation.

## Extending the System

### Adding New Patterns

Add patterns to `mask.hpp`:
```cpp
constexpr auto MY_PATTERN = parse_pattern("xxxxx.goooooo");
```

### Adding Pattern Groups

Add to the appropriate decision function:
```cpp
constexpr std::array<Rule, N> MY_GROUP {{
    { MY_PATTERN, Action::MOVE_UP },
    // ... more rules
}};
```

### Custom Agent Types

Extend `Agent::decide()` to handle new robot colors with different rule sets.

## Grid Representation

- `x` = Boundary (edges of grid)
- `.` = Empty cell
- `g` = Green robot
- `r` = Red robot
- `b` = Blue robot

## Testing

The `main.cpp` includes test:

# mask.hpp — Local View Masking and Rule Matching for Grid Robots

This header provides a compact, reusable way to represent and match **local views** of robots on a 2D grid using bitmasks. It is designed to implement the view–pattern–action rules from the paper:

> _Maximum independent set formation on a finite grid by myopic robots_ (Das, Sharma, Sau, TCS 2025)

The core idea:

- Each robot sees a **2-hop neighborhood** around itself (13 relevant cells).
- Each cell is encoded as a 5-bit mask (`CellMask`) representing:
  - boundary (no node),
  - empty node,
  - or a coloured robot (blue, red, green).
- Patterns are written as **13-character strings** using a small DSL (`parse_pattern`).
- Rules (`Rule`) map a pattern mask to an **action** (`Action`):
  - move in one of 4 directions,
  - change color (green ↔ blue ↔ red),
  - or combinations (e.g. move + recolor for blue robots).
- The simulator matches a robot’s current view against rule sets in a fixed **priority order**, faithfully following Algorithm 1 of the paper.

---

## Coordinate & Indexing Model

Each robot’s local view is represented as a 13-cell vector in this fixed order:

```text
Index   Symbol   Meaning       Relative position
-----   ------   ---------     -----------------
  0       U2     Up 2 steps         (0, -2)
  1       UL     Up-left            (-1, -1)
  2       U1     Up 1 step          (0, -1)
  3       UR     Up-right           (+1, -1)
  4       L2     Left 2 steps       (-2, 0)
  5       L1     Left 1 step        (-1, 0)
  6       C      Center (self)      (0, 0)
  7       R1     Right 1 step       (+1, 0)
  8       R2     Right 2 steps      (+2, 0)
  9       DL     Down-left          (-1, +1)
 10       D1     Down 1 step        (0, +1)
 11       DR     Down-right         (+1, +1)
 12       D2     Down 2 steps       (0, +2)
