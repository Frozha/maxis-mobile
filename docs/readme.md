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
