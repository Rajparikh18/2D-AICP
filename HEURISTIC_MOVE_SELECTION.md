# Heuristic-Based Move Selection Implementation

## Overview
Replaced random move selection with intelligent heuristic-based move ordering in both **Minimax** and **Monte Carlo** algorithms. This makes the AI strategically prioritize moves that improve its winning chances while blocking opponent paths.

## Core Heuristic Formula

```
Score(move) = MyConnectivityImprovement - OpponentConnectivityImprovement
```

Where:
- **MyConnectivityImprovement** = Connectivity after move - Connectivity before move
- **OpponentConnectivityImprovement** = Opponent's connectivity change after our move

### Why This Works
1. **Offensive**: Moves that improve our connectivity get higher scores
2. **Defensive**: Moves that block opponent's path (reducing their connectivity) get higher scores
3. **Dual Purpose**: Best moves both advance our position AND block the opponent

## Implementation Details

### 1. New Methods Added

Both `Minimax` and `MonteCarlo` classes now have:

#### `scoreMoveHeuristic(HexGrid& grid, const HexCoord& move, Player player)`
- Temporarily makes a move
- Calculates connectivity changes for both players
- Returns: `myImprovement - opponentImprovement`
- Higher score = better move

#### `orderMovesByHeuristic(HexGrid& grid, const std::vector<HexCoord>& moves, Player player)`
- Scores all candidate moves using `scoreMoveHeuristic()`
- Sorts moves by score (highest first)
- Returns ordered list of moves

### 2. Changes in Minimax Algorithm

**Before:**
```cpp
std::shuffle(emptyCells.begin(), emptyCells.end(), gen);  // Random ordering
```

**After:**
```cpp
emptyCells = orderMovesByHeuristic(grid, emptyCells, player);  // Strategic ordering
```

**Impact:**
- Alpha-beta pruning now evaluates best moves first
- Better pruning efficiency → faster search
- More likely to find optimal move within search depth

### 3. Changes in Monte Carlo Algorithm

**Before:**
```cpp
std::shuffle(emptyCells.begin(), emptyCells.end(), rng);  // Random selection
```

**After:**
```cpp
emptyCells = orderMovesByHeuristic(grid, emptyCells, player);  // Strategic ordering
```

**Impact:**
- Simulations focus on promising moves
- Better statistical distribution
- Higher win rate predictions for good moves

## Key Benefits

### 1. **Strategic Move Selection**
- AI no longer wastes time on weak moves
- Focuses computational resources on promising positions

### 2. **Automatic Blocking**
- When opponent is close to winning (high connectivity), blocking moves naturally score higher
- AI becomes more defensive when needed

### 3. **Better Path Formation**
- AI actively seeks moves that extend its own winning path
- Creates bridges and connections more efficiently

### 4. **Performance Gains**
- **Minimax**: Better alpha-beta pruning → fewer nodes evaluated
- **Monte Carlo**: Better move candidates → more accurate win rate estimation

## Technical Notes

### Namespace Isolation
To avoid struct name conflicts, `MoveScore` is defined in separate namespaces:
- `MinimaxInternal::MoveScore`
- `MonteCarloInternal::MoveScore`

### Connectivity Calculation
Uses existing `PathFinding::calculateConnectivity()` which:
- Returns distance from start edge to goal edge
- Higher value = closer to winning
- Accounts for both placed stones and empty cells

### Move Ordering Efficiency
- Only scores moves once per search level
- Uses efficient sorting (O(n log n))
- Cached via ordered vector for sequential access

## Expected Behavior

### Offensive Play
When the AI has the advantage:
- Prioritizes moves extending its winning path
- Creates strong connections between stones
- Advances toward goal edge

### Defensive Play
When opponent threatens to win:
- Automatically identifies blocking positions
- Places stones to disrupt opponent's connectivity
- Balances offense and defense

### Balanced Play
In neutral positions:
- Evaluates both offensive and defensive merits
- Chooses moves that maximize net advantage
- Maintains strategic flexibility

## Testing Recommendations

1. **Watch for defensive behavior**: AI should block opponent's strong paths
2. **Check path formation**: AI should build connected paths efficiently  
3. **Verify winning plays**: AI should recognize and execute winning moves
4. **Compare to random**: Should see noticeable improvement over previous random selection

## Future Enhancements

Potential improvements to the heuristic:
- Weight connectivity by distance to goal (closer = more urgent)
- Consider multiple path options (redundancy)
- Add bridge pattern detection
- Incorporate threat recognition (immediate wins/losses)
