#pragma once
#include<cstdint>
#include<array>
#include<cstddef>
constexpr int VIEW_LEN = 13;
enum class CellMask : uint8_t {
    // --- Concrete single states (one-hot) ---
    BOU  = 0b00001, //BOUNDARY (CROSS)  -> x
    BLU  = 0b00010, //BLUE              -> b
    RED  = 0b00100, //RED               -> r
    GRE  = 0b01000, //GREEN             -> g
    EMP  = 0b10000, //EMPTY             -> .

    // --- Meta masks (multi-state sets) ---
    ANR = 0b01110, // BLU | RED | GRE, //ANY_ROBOT   N/A
    EXI = 0b11110, // ANR | EMP // EXISTS (DIAMOND) -> d
    ANY = 0b11111  // EXI | BOU //ANY (EMPTY CIRCLE) -> o
};

//fast way to generate masks
constexpr CellMask char_to_mask(char c) {
    switch (c) {
        case 'o': return CellMask::ANY;
        case 'x': return CellMask::BOU;
        case 'd': return CellMask::EXI;
        case '.': return CellMask::EMP;
        case 'g': return CellMask::GRE;
        case 'r': return CellMask::RED;
        case 'b': return CellMask::BLU;
    }
}
using View = std::array<CellMask, VIEW_LEN>;
constexpr View parse_pattern(const char (&s)[VIEW_LEN+1]) {
    View p{};
    for (int i = 0; i < VIEW_LEN; ++i)
        p[i] = char_to_mask(s[i]);
    return p;
}

//for comparision of masks
inline constexpr CellMask operator|(CellMask a, CellMask b) {
    return CellMask(uint8_t(a) | uint8_t(b));
}
inline constexpr CellMask operator&(CellMask a, CellMask b) {
    return CellMask(uint8_t(a) & uint8_t(b));
}
inline constexpr CellMask& operator|=(CellMask& a, CellMask b) {
    a = a | b; return a;
}


//index of the mask and position mapping in the array
enum class ViewIndex : uint8_t {
    U2  = 0,
    UL  = 1,
    U1  = 2,
    UR  = 3,
    L2  = 4,
    L1  = 5,
    C   = 6,   
    R1  = 7,
    R2  = 8,
    DL  = 9,
    D1  = 10,
    DR  = 11,
    D2  = 12
};

//matching logic
//returns whether view matches mask
constexpr bool match_view(const View& view, const View& mask) {
    for (int i = 0; i < VIEW_LEN; i++)
        if ((uint8_t)(view[i] & mask[i]) == 0)
            return false;
    return true;
}
constexpr bool match_view(const CellMask view[VIEW_LEN], const CellMask mask[VIEW_LEN]) {
    for (int i = 0; i < VIEW_LEN; i++)
        if ((uint8_t)(view[i] & mask[i]) == 0)
            return false;
    return true;
}

enum class Action: uint8_t {
    NONE         = 0,
    MOVE_LEFT    = 0b0000001,
    MOVE_RIGHT   = 0b0000010,
    MOVE_UP      = 0b0000100,
    MOVE_DOWN    = 0b0001000,
    BECOME_BLUE  = 0b0010000,
    BECOME_RED   = 0b0100000,
    BECOME_GREEN = 0b1000000,
};

inline constexpr Action operator|(Action a, Action b) {
    return Action(uint8_t(a) | uint8_t(b));
}
inline constexpr Action operator&(Action a, Action b) {
    return Action(uint8_t(a) & uint8_t(b));
}
inline constexpr Action& operator|=(Action& a, Action b) {
    a = a | b; return a;
}
inline constexpr bool has(Action a, Action flag) {
    return uint8_t(a & flag) != 0;
}
struct Rule {
    View mask;
    Action action;
};



//G1 = {GL1, GL2, GL3, GL4}
//Fig. 7. Views of Green robot to move Left.
constexpr auto GL1 = parse_pattern("oooo..goooooo");
constexpr auto GL2 = parse_pattern("o.oor.goooooo");
constexpr auto GL3 = parse_pattern("o.oox.goooooo");
constexpr auto GL4 = parse_pattern("xxxxx.goooooo");
constexpr std::array<Rule, 4> G1 {{
    { GL1, Action::MOVE_LEFT },
    { GL2, Action::MOVE_LEFT },
    { GL3, Action::MOVE_LEFT },
    { GL4, Action::MOVE_LEFT },
}};


//G2 = {GD1, GD2, GD3, GD4}
//Fig. 8. Views of Green robot to move Downward due to red robot.
constexpr auto GD1 = parse_pattern("xxxxorgxxo.xo");
constexpr auto GD2 = parse_pattern("oorxorgxxo.xo");
//Fig. 9. View of Green robot to move Downward when a blue robot is at left
constexpr auto GD3 = parse_pattern("oooxobrxxo.xo");
//Fig. 10. View of Green robot to move Downward when a blue robot is at upward
constexpr auto GD4 = parse_pattern("oobxoorxxo.xo");
constexpr std::array<Rule, 4> G2 {{
    { GD1, Action::MOVE_DOWN },
    { GD2, Action::MOVE_DOWN },
    { GD3, Action::MOVE_DOWN },
    { GD4, Action::MOVE_DOWN },
}};



//G3 = {GR1, GR2, GR3, GR4, GR5, GR6, GR7, GR8}
//Fig. 11. Views of Green robot to move Right due to red robot
constexpr auto GR1 = parse_pattern("oxr.xxg.xxooo");
constexpr auto GR2 = parse_pattern("oxroxxg.dxooo");
constexpr auto GR3 = parse_pattern("xxxxorg.ooooo");
constexpr auto GR4 = parse_pattern("oor.org.xoooo");
//Fig. 12. Views of Green robot to move Right due to blue robot.
constexpr auto GR6 = parse_pattern("ooooobg.doooo");
constexpr auto GR7 = parse_pattern("ooo.obg.xoooo");
constexpr auto GR8 = parse_pattern("ooorobg.xoooo");
constexpr std::array<Rule, 7> G3 {{
    { GR1, Action::MOVE_RIGHT },
    { GR2, Action::MOVE_RIGHT },
    { GR3, Action::MOVE_RIGHT },
    { GR4, Action::MOVE_RIGHT },
    { GR6, Action::MOVE_RIGHT },
    { GR7, Action::MOVE_RIGHT },
    { GR8, Action::MOVE_RIGHT },
}};




//G4 = {GU1, ... GU20}
//Fig. 13. Views of Green robot to move Upward due to red robot.
constexpr auto GU1 = parse_pattern(".x..xxgooxooo");
constexpr auto GU2 = parse_pattern("xx..xxgooxooo");
constexpr auto GU3 = parse_pattern("....xrgoooooo");
constexpr auto GU4 = parse_pattern(".r..x.goooooo");
constexpr auto GU5 = parse_pattern(".r..r.goooooo");
constexpr auto GU6 = parse_pattern(".....rgoooooo");
constexpr auto GU7 = parse_pattern("x....rgoooooo");
constexpr auto GU8 = parse_pattern("...xxrgxxooxo");
constexpr auto GU9 = parse_pattern(".r.xx.gxxooxo");
constexpr auto GU10 = parse_pattern(".r.xr.gxxooxo");
constexpr auto GU11 = parse_pattern("...x.rgxxooxo");
constexpr auto GU12 = parse_pattern("x..x.rgxxooxo");
//Fig. 14. Views of Green robot to move Upward due to blue robot.
constexpr auto GU13 = parse_pattern("x...obgoooooo");
constexpr auto GU14 = parse_pattern("x..xobgxxooxo");
constexpr auto GU15 = parse_pattern("....obgoooooo");
constexpr auto GU16 = parse_pattern("...xobgxxooxo");
constexpr auto GU17 = parse_pattern("...xobgxxxxxx");
constexpr auto GU18 = parse_pattern(".r..obgoooooo");
constexpr auto GU19 = parse_pattern(".r.xobgxxooxo");
constexpr auto GU20 = parse_pattern(".r.xobgxxxxxx");
constexpr std::array<Rule, 20> G4 {{
    { GU1,  Action::MOVE_UP }, { GU2,  Action::MOVE_UP },
    { GU3,  Action::MOVE_UP }, { GU4,  Action::MOVE_UP },
    { GU5,  Action::MOVE_UP }, { GU6,  Action::MOVE_UP },
    { GU7,  Action::MOVE_UP }, { GU8,  Action::MOVE_UP },
    { GU9,  Action::MOVE_UP }, { GU10, Action::MOVE_UP },
    { GU11, Action::MOVE_UP }, { GU12, Action::MOVE_UP },
    { GU13, Action::MOVE_UP }, { GU14, Action::MOVE_UP },
    { GU15, Action::MOVE_UP }, { GU16, Action::MOVE_UP },
    { GU17, Action::MOVE_UP }, { GU18, Action::MOVE_UP },
    { GU19, Action::MOVE_UP }, { GU20, Action::MOVE_UP },
}};




//G5 = {GB1,... GB14}
//Fig. 15. Views of Green robot to become Blue due to red robot.
constexpr auto GB1 = parse_pattern("oxroxxggoxooo");
constexpr auto GB2 = parse_pattern("xxxxorggooooo");
constexpr auto GB3 = parse_pattern("xxxxorgxxogxo");
constexpr auto GB4 = parse_pattern("ooroorggooooo");
constexpr auto GB5 = parse_pattern("oorxorgxxogxo");
//Fig. 16. Views of Green robot to become Blue when a blue robot is at left.
constexpr auto GB6 = parse_pattern("rr.oobggooooo");
constexpr auto GB7 = parse_pattern("rr.xobgxxogxo");
constexpr auto GB8 = parse_pattern("xr.oobggooooo");
constexpr auto GB9 = parse_pattern("xr.xobgxxogxo");
constexpr auto GB10 = parse_pattern("ooroobggooooo");
constexpr auto GB11 = parse_pattern("oorxobgxxogxo");
constexpr auto GB12 = parse_pattern("xxxxobggooooo");
constexpr auto GB13 = parse_pattern("xxxxobgxxogxo");
//Fig. 17. View of Green robot to become Blue when a blue robot is at upward.
constexpr auto GB14 = parse_pattern("oobxoogxxogxo");
constexpr std::array<Rule, 14> G5 {{
    { GB1,  Action::BECOME_BLUE }, { GB2,  Action::BECOME_BLUE },
    { GB3,  Action::BECOME_BLUE }, { GB4,  Action::BECOME_BLUE },
    { GB5,  Action::BECOME_BLUE }, { GB6,  Action::BECOME_BLUE },
    { GB7,  Action::BECOME_BLUE }, { GB8,  Action::BECOME_BLUE },
    { GB9,  Action::BECOME_BLUE }, { GB10, Action::BECOME_BLUE },
    { GB11, Action::BECOME_BLUE }, { GB12, Action::BECOME_BLUE },
    { GB13, Action::BECOME_BLUE }, { GB14, Action::BECOME_BLUE },
}};


//G6 = {G-R1, G-R2, G-R3, G-R4, G-R5, G-R6, G-R7}
//Fig. 18. Views of Green robot to become Red.
constexpr auto G_R1 = parse_pattern("xxxxxxgooxooo");
constexpr auto G_R2 = parse_pattern("rx.oxxgooxooo");
constexpr auto G_R3 = parse_pattern("xr.ox.goooooo");
constexpr auto G_R4 = parse_pattern("xxxxr.goooooo");
constexpr auto G_R5 = parse_pattern("rr.ox.goooooo");
constexpr auto G_R6 = parse_pattern("rr.or.goooooo");
constexpr auto G_R7 = parse_pattern("xr.or.goooooo");
constexpr std::array<Rule, 7> G6 {{
    { G_R1, Action::BECOME_RED },
    { G_R2, Action::BECOME_RED },
    { G_R3, Action::BECOME_RED },
    { G_R4, Action::BECOME_RED },
    { G_R5, Action::BECOME_RED },
    { G_R6, Action::BECOME_RED },
    { G_R7, Action::BECOME_RED },
}};


//B1 = {BGR1, BGR2, BGR3}
//Fig. 19. Views of Blue robot to become Green and move Right
constexpr auto BGR1 = parse_pattern("oooooob.doooo");
constexpr auto BGR2 = parse_pattern("ooo.oob.xoooo");
constexpr auto BGR3 = parse_pattern("oooroob.xoooo");
constexpr std::array<Rule, 3> B1 {{
    { BGR1, Action::BECOME_GREEN | Action::MOVE_RIGHT},
    { BGR2, Action::BECOME_GREEN | Action::MOVE_RIGHT},
    { BGR3, Action::BECOME_GREEN | Action::MOVE_RIGHT},
}};


//B2 = {BGL1}
//Fig. 20. View of Blue robot to become Green and move Left
constexpr auto BGL1 = parse_pattern("ooox..bxxooxo");
constexpr std::array<Rule, 1> B2 {{
    { BGL1, Action::BECOME_GREEN | Action::MOVE_LEFT},
}};


//B3 = {BGD1}
//Fig. 21. View of Blue robot to become Green and move Downward
constexpr auto BGD1 = parse_pattern("oooxoobxxo.xo");
constexpr std::array<Rule, 1> B3 {{
    { BGD1, Action::BECOME_GREEN | Action::MOVE_DOWN},
}};

//B4 = {BG1, BG2, BG3, BG4, BG5}
//Fig. 22. Views of Blue robot to become Green
constexpr auto BG1 = parse_pattern("oo.xo.bxxooxo");
constexpr auto BG2 = parse_pattern("oo.xogbxxooxo");
constexpr auto BG3 = parse_pattern("oo.xorbxxooxo");
constexpr auto BG4 = parse_pattern("oogxoobxxooxo");
constexpr auto BG5 = parse_pattern("oorxo.bxxooxo");
constexpr std::array<Rule, 5> B4 {{
    { BG1, Action::BECOME_GREEN },
    { BG2, Action::BECOME_GREEN },
    { BG3, Action::BECOME_GREEN },
    { BG4, Action::BECOME_GREEN },
    { BG5, Action::BECOME_GREEN },
}};



template <size_t N>
constexpr Action match_rules(const View& view,
                             const std::array<Rule, N>& rules){
    for (const auto& r : rules) {
        if (match_view(view, r.mask)) {
            return r.action;
        }
    }
    return Action::NONE;
}

Action decide_green(const View& view) {
    if (auto a = match_rules(view, G1); a != Action::NONE) return a; // move left
    if (auto a = match_rules(view, G2); a != Action::NONE) return a; // move down
    if (auto a = match_rules(view, G3); a != Action::NONE) return a; // move right
    if (auto a = match_rules(view, G4); a != Action::NONE) return a; // move up
    if (auto a = match_rules(view, G5); a != Action::NONE) return a; // become blue
    if (auto a = match_rules(view, G6); a != Action::NONE) return a; // become red
    return Action::NONE;
}

Action decide_blue(const View& view) {
    if (auto a = match_rules(view, B1); a != Action::NONE) return a;
    if (auto a = match_rules(view, B2); a != Action::NONE) return a;
    if (auto a = match_rules(view, B3); a != Action::NONE) return a;
    if (auto a = match_rules(view, B4); a != Action::NONE) return a;

    return Action::NONE;
}




























/*
using Pattern = View;

struct PatternDesc {
    const char* name;
    const Pattern* mask;
};

const PatternDesc ALL_PATTERNS[] = {
    // G1 = {GL1, GL2, GL3, GL4}
    { "GL1", &GL1 }, { "GL2", &GL2 }, { "GL3", &GL3 }, { "GL4", &GL4 },

    // G2 = {GD1, GD2, GD3, GD4}
    { "GD1", &GD1 }, { "GD2", &GD2 }, { "GD3", &GD3 }, { "GD4", &GD4 },

    // G3 = {GR1, GR2, GR3, GR4, GR6, GR7, GR8}
    { "GR1", &GR1 }, { "GR2", &GR2 }, { "GR3", &GR3 }, { "GR4", &GR4 },
    { "GR6", &GR6 }, { "GR7", &GR7 }, { "GR8", &GR8 },

    // G4 = {GU1 ... GU20}
    { "GU1", &GU1 }, { "GU2", &GU2 }, { "GU3", &GU3 }, { "GU4", &GU4 },
    { "GU5", &GU5 }, { "GU6", &GU6 }, { "GU7", &GU7 }, { "GU8", &GU8 },
    { "GU9", &GU9 }, { "GU10", &GU10 }, { "GU11", &GU11 }, { "GU12", &GU12 },
    { "GU13", &GU13 }, { "GU14", &GU14 }, { "GU15", &GU15 }, { "GU16", &GU16 },
    { "GU17", &GU17 }, { "GU18", &GU18 }, { "GU19", &GU19 }, { "GU20", &GU20 },

    // G5 = {GB1 ... GB14}
    { "GB1", &GB1 }, { "GB2", &GB2 }, { "GB3", &GB3 }, { "GB4", &GB4 },
    { "GB5", &GB5 }, { "GB6", &GB6 }, { "GB7", &GB7 }, { "GB8", &GB8 },
    { "GB9", &GB9 }, { "GB10", &GB10 }, { "GB11", &GB11 }, { "GB12", &GB12 },
    { "GB13", &GB13 }, { "GB14", &GB14 },

    // G6 = {G_R1 ... G_R7}
    { "G_R1", &G_R1 }, { "G_R2", &G_R2 }, { "G_R3", &G_R3 },
    { "G_R4", &G_R4 }, { "G_R5", &G_R5 }, { "G_R6", &G_R6 }, { "G_R7", &G_R7 },

    // B1 = {BGR1, BGR2, BGR3}
    { "BGR1", &BGR1 }, { "BGR2", &BGR2 }, { "BGR3", &BGR3 },

    // B2 = {BGL1}
    { "BGL1", &BGL1 },

    // B3 = {BGD1}
    { "BGD1", &BGD1 },

    // B4 = {BG1 ... BG5}
    { "BG1", &BG1 }, { "BG2", &BG2 }, { "BG3", &BG3 },
    { "BG4", &BG4 }, { "BG5", &BG5 },
};

constexpr size_t NUM_PATTERNS = sizeof(ALL_PATTERNS) / sizeof(ALL_PATTERNS[0]);

void dump_overlapping_patterns() {
    bool found = false;
    for (size_t i = 0; i < NUM_PATTERNS; ++i) {
        for (size_t j = i + 1; j < NUM_PATTERNS; ++j) {
            if (match_view(*ALL_PATTERNS[i].mask, *ALL_PATTERNS[j].mask)) {
                found = true;
                std::cout << "Overlap: " 
                          << ALL_PATTERNS[i].name 
                          << "  <-->  " 
                          << ALL_PATTERNS[j].name 
                          << '\n';
            }
        }
    }
    if (!found) {
        std::cout << "No overlapping patterns found.\n";
    }
}
int main() {
    dump_overlapping_patterns();
    return 0;
}


int main(){
    
    auto view = parse_pattern("rooo..goooooo");

    cout<<match_view(GL1, view);
    return 0;

}*/