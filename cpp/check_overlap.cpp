// check_overlap.cpp
// For each pair of rules (G1-G6, B1-B4, CUST1-5), checks whether there exists
// any concrete 13-cell view that would match both patterns simultaneously.
// Two patterns overlap iff for every position i: (maskA[i] & maskB[i]) != 0
// i.e. match_view(maskA, maskB) is true (the check is symmetric).
//
// For overlapping pairs with DIFFERENT actions the higher-priority rule silently
// suppresses the other — this is where deadlocks can originate.
//
// Priority order (first match wins):
//   Green: G1 > G2 > G3 > G4 > G5 > G6
//   Blue:  B1 > B2 > B3 > B4
//   Custom rules only fire when all original rules return NONE.
//
// Compile: g++ -std=c++17 -I headers -o check_overlap check_overlap.cpp
// Run:     ./check_overlap

#include "headers/mask.hpp"
#include <iostream>
#include <string>
#include <vector>

struct PatternDesc {
    std::string name;
    View        mask;
    Action      action;
    int         priority; // lower = checked first
    bool        is_custom;
};

static std::string action_str(Action a) {
    if (a == Action::NONE) return "NONE";
    std::string s;
    if (has(a, Action::BECOME_GREEN)) s += "BECOME_GREEN|";
    if (has(a, Action::BECOME_BLUE))  s += "BECOME_BLUE|";
    if (has(a, Action::BECOME_RED))   s += "BECOME_RED|";
    if (has(a, Action::MOVE_LEFT))    s += "MOVE_LEFT|";
    if (has(a, Action::MOVE_RIGHT))   s += "MOVE_RIGHT|";
    if (has(a, Action::MOVE_UP))      s += "MOVE_UP|";
    if (has(a, Action::MOVE_DOWN))    s += "MOVE_DOWN|";
    if (!s.empty()) s.pop_back();
    return s;
}

static const std::vector<PatternDesc> ALL_PATTERNS = {
    // G1 (priority 1) – move left
    { "GL1", GL1, Action::MOVE_LEFT, 1, false },
    { "GL2", GL2, Action::MOVE_LEFT, 1, false },
    { "GL3", GL3, Action::MOVE_LEFT, 1, false },
    { "GL4", GL4, Action::MOVE_LEFT, 1, false },
    // G2 (priority 2) – move down
    { "GD1", GD1, Action::MOVE_DOWN, 2, false },
    { "GD2", GD2, Action::MOVE_DOWN, 2, false },
    { "GD3", GD3, Action::MOVE_DOWN, 2, false },
    { "GD4", GD4, Action::MOVE_DOWN, 2, false },
    // G3 (priority 3) – move right
    { "GR1", GR1, Action::MOVE_RIGHT, 3, false },
    { "GR2", GR2, Action::MOVE_RIGHT, 3, false },
    { "GR3", GR3, Action::MOVE_RIGHT, 3, false },
    { "GR4", GR4, Action::MOVE_RIGHT, 3, false },
    { "GR5", GR5, Action::MOVE_RIGHT, 3, false },
    { "GR6", GR6, Action::MOVE_RIGHT, 3, false },
    { "GR7", GR7, Action::MOVE_RIGHT, 3, false },
    { "GR8", GR8, Action::MOVE_RIGHT, 3, false },
    // G4 (priority 4) – move up
    { "GU1",  GU1,  Action::MOVE_UP, 4, false },
    { "GU2",  GU2,  Action::MOVE_UP, 4, false },
    { "GU3",  GU3,  Action::MOVE_UP, 4, false },
    { "GU4",  GU4,  Action::MOVE_UP, 4, false },
    { "GU5",  GU5,  Action::MOVE_UP, 4, false },
    { "GU6",  GU6,  Action::MOVE_UP, 4, false },
    { "GU7",  GU7,  Action::MOVE_UP, 4, false },
    { "GU8",  GU8,  Action::MOVE_UP, 4, false },
    { "GU9",  GU9,  Action::MOVE_UP, 4, false },
    { "GU10", GU10, Action::MOVE_UP, 4, false },
    { "GU11", GU11, Action::MOVE_UP, 4, false },
    { "GU12", GU12, Action::MOVE_UP, 4, false },
    { "GU13", GU13, Action::MOVE_UP, 4, false },
    { "GU14", GU14, Action::MOVE_UP, 4, false },
    { "GU15", GU15, Action::MOVE_UP, 4, false },
    { "GU16", GU16, Action::MOVE_UP, 4, false },
    { "GU17", GU17, Action::MOVE_UP, 4, false },
    { "GU18", GU18, Action::MOVE_UP, 4, false },
    { "GU19", GU19, Action::MOVE_UP, 4, false },
    { "GU20", GU20, Action::MOVE_UP, 4, false },
    // G5 (priority 5) – become blue
    { "GB1",  GB1,  Action::BECOME_BLUE, 5, false },
    { "GB2",  GB2,  Action::BECOME_BLUE, 5, false },
    { "GB3",  GB3,  Action::BECOME_BLUE, 5, false },
    { "GB4",  GB4,  Action::BECOME_BLUE, 5, false },
    { "GB5",  GB5,  Action::BECOME_BLUE, 5, false },
    { "GB6",  GB6,  Action::BECOME_BLUE, 5, false },
    { "GB7",  GB7,  Action::BECOME_BLUE, 5, false },
    { "GB8",  GB8,  Action::BECOME_BLUE, 5, false },
    { "GB9",  GB9,  Action::BECOME_BLUE, 5, false },
    { "GB10", GB10, Action::BECOME_BLUE, 5, false },
    { "GB11", GB11, Action::BECOME_BLUE, 5, false },
    { "GB12", GB12, Action::BECOME_BLUE, 5, false },
    { "GB13", GB13, Action::BECOME_BLUE, 5, false },
    { "GB14", GB14, Action::BECOME_BLUE, 5, false },
    // G6 (priority 6) – become red
    { "G_R1", G_R1, Action::BECOME_RED, 6, false },
    { "G_R2", G_R2, Action::BECOME_RED, 6, false },
    { "G_R3", G_R3, Action::BECOME_RED, 6, false },
    { "G_R4", G_R4, Action::BECOME_RED, 6, false },
    { "G_R5", G_R5, Action::BECOME_RED, 6, false },
    { "G_R6", G_R6, Action::BECOME_RED, 6, false },
    { "G_R7", G_R7, Action::BECOME_RED, 6, false },
    // B1 (priority 1) – become green + move right
    { "BGR1", BGR1, Action::BECOME_GREEN | Action::MOVE_RIGHT, 1, false },
    { "BGR2", BGR2, Action::BECOME_GREEN | Action::MOVE_RIGHT, 1, false },
    { "BGR3", BGR3, Action::BECOME_GREEN | Action::MOVE_RIGHT, 1, false },
    // B2 (priority 2) – become green + move left
    { "BGL1", BGL1, Action::BECOME_GREEN | Action::MOVE_LEFT, 2, false },
    // B3 (priority 3) – become green + move down
    { "BGD1", BGD1, Action::BECOME_GREEN | Action::MOVE_DOWN, 3, false },
    // B4 (priority 4) – become green (in place)
    { "BG1", BG1, Action::BECOME_GREEN, 4, false },
    { "BG2", BG2, Action::BECOME_GREEN, 4, false },
    { "BG3", BG3, Action::BECOME_GREEN, 4, false },
    { "BG4", BG4, Action::BECOME_GREEN, 4, false },
    { "BG5", BG5, Action::BECOME_GREEN, 4, false },
    // Custom rules (priority 99 — only fire when all originals return NONE)
    // CUST1,5: green robot (C='g'); CUST2,3,4: blue robot (C='b')
    { "CUST1", CUST1, Action::MOVE_RIGHT,                          99, true },
    { "CUST2", CUST2, Action::MOVE_RIGHT | Action::BECOME_GREEN,   99, true },
    { "CUST3", CUST3, Action::MOVE_DOWN  | Action::BECOME_GREEN,   99, true },
    { "CUST4", CUST4, Action::MOVE_RIGHT | Action::BECOME_GREEN,   99, true },
    { "CUST5", CUST5, Action::BECOME_BLUE,                         99, true },
};

struct OverlapInfo {
    std::string nameA, nameB;
    Action actionA, actionB;
    int priorityA, priorityB; // which fires first (lower wins)
    bool is_conflict;          // true when actions differ
    bool involves_custom;
};

int main() {
    int n = static_cast<int>(ALL_PATTERNS.size());
    std::cout << "Total rules: " << n << " (67 original + 5 custom)\n\n";

    std::vector<OverlapInfo> harmless, conflicts_orig, conflicts_cust;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (!match_view(ALL_PATTERNS[i].mask, ALL_PATTERNS[j].mask))
                continue;

            const auto& a = ALL_PATTERNS[i];
            const auto& b = ALL_PATTERNS[j];
            bool conflict       = (a.action != b.action);
            bool involves_cust  = (a.is_custom || b.is_custom);

            OverlapInfo info { a.name, b.name,
                               a.action, b.action,
                               a.priority, b.priority,
                               conflict, involves_cust };

            if (!conflict)              harmless.push_back(info);
            else if (!involves_cust)    conflicts_orig.push_back(info);
            else                        conflicts_cust.push_back(info);
        }
    }

    // --- Harmless overlaps (same action, priority irrelevant) ---
    std::cout << "=== Same-action overlaps (harmless, " << harmless.size() << ") ===\n";
    for (auto& o : harmless)
        std::cout << "  " << o.nameA << "  <-->  " << o.nameB
                  << "  [both: " << action_str(o.actionA) << "]\n";

    // --- Conflicting overlaps among original rules ---
    std::cout << "\n=== Action-conflict overlaps in original rules ("
              << conflicts_orig.size() << ") ===\n";
    std::cout << "  Format: WINNER (fired) suppresses LOSER (suppressed) : action difference\n\n";
    for (auto& o : conflicts_orig) {
        // lower priority number = checked first = wins
        bool a_wins = (o.priorityA <= o.priorityB);
        std::string winner  = a_wins ? o.nameA : o.nameB;
        std::string loser   = a_wins ? o.nameB : o.nameA;
        Action w_act = a_wins ? o.actionA : o.actionB;
        Action l_act = a_wins ? o.actionB : o.actionA;
        std::cout << "  " << winner << " [" << action_str(w_act) << "]"
                  << "  suppresses  "
                  << loser  << " [" << action_str(l_act) << "]\n";
    }

    // --- Conflicting overlaps involving custom rules ---
    std::cout << "\n=== Action-conflict overlaps involving custom rules ("
              << conflicts_cust.size() << ") ===\n";
    if (conflicts_cust.empty())
        std::cout << "  None — custom rules only fire in views no original rule covers.\n";
    else
        for (auto& o : conflicts_cust)
            std::cout << "  " << o.nameA << " [" << action_str(o.actionA) << "]"
                      << "  <-->  "
                      << o.nameB << " [" << action_str(o.actionB) << "]\n";

    std::cout << "\nTotal overlapping pairs: "
              << (harmless.size() + conflicts_orig.size() + conflicts_cust.size()) << "\n";
    return 0;
}
