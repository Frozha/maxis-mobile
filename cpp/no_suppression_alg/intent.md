# no_suppression_alg — Intent

## Hypothesis
The original algorithm's 24 action-conflicting rule overlaps are resolved by a fixed priority ordering (G1 > G2 > ... > G6, B1 > ... > B4). Three of these suppressions were traced as direct causes of the 5 deadlock configurations patched by the custom rules:

- **GL1/2/3 suppress GB14** — robots near the right boundary move left instead of becoming blue, drifting into CUST1/2/4 deadlock views
- **GL1/2/3 suppress GD4** — robots near the right boundary move left instead of downward, contributing to CUST4
- **GR6/7 suppress GU13/15/18** — robots with blue to their left move right instead of upward, accumulating in the red mass and producing CUST3/5

The hypothesis is that elevating these three suppressed rules above their suppressors is sufficient to prevent the system from ever reaching those deadlock views — making the custom rules unnecessary.

## What this folder tests
`suppression_aware_decide_green()` checks GB14, GD4, GU13, GU15, and GU18 *before* the standard cascade. All other rules and their original ordering are unchanged. Blue rules are also unchanged. No custom rules are included as a fallback.

This isolates the single variable of priority ordering. If the success rate reaches 100%, the conclusion is that the deadlocks were a consequence of the priority ordering, not of missing coverage in the rule set. If it remains below 100%, other gaps exist independent of the suppression chains.

## Comparison baseline
- Unmodified algorithm: ~62.84% mean success rate (15×15 to 25×25, 10,000 sims each)
- Modified start (empty top row): 100%
- Modified algorithm (custom rules + collision handling): 100%

## To run
```bash
cd cpp/no_suppression_alg
g++ -std=c++17 -I ../headers -o no_suppression_mass no_suppression_mass_simulator.cpp
./no_suppression_mass 20 20 1000 quiet
```
