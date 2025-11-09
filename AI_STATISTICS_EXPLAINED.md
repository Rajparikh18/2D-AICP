# AI Statistics Explained 🤖

## What You See in the Game

When the AI (BLUE) makes a move, the game shows these statistics:

```
--- AI Statistics ---
Think time: 53 ms
Nodes: 152
Simulations: 300
Win rate: 0.0%
Score: 3.5
```

## What Each Statistic Means

### 1. Think Time (milliseconds)
**What it is:** How long the AI took to calculate its move

**Example:** `Think time: 53 ms` = AI thought for 53 milliseconds (0.053 seconds)

**What it tells you:**
- EASY mode: Usually 20-50 ms (very fast)
- MEDIUM mode: Usually 50-150 ms (fast)
- HARD mode: Usually 150-500 ms (slower, but still quick)

**Why it matters:** Shows how complex the AI's calculation was. Longer think time = AI analyzed more possibilities.

---

### 2. Nodes (positions evaluated)
**What it is:** Number of game positions the **Minimax algorithm** analyzed

**Example:** `Nodes: 152` = Minimax looked at 152 different possible board positions

**What it tells you:**
- More nodes = AI looked deeper into the future
- Minimax explores the "game tree" of possible moves
- Each node is one possible future board state

**How it works:**
```
Current Board
├── If I play here...
│   ├── If opponent plays here...
│   │   ├── If I play here... (3 moves ahead)
│   │   └── If I play there...
│   └── If opponent plays there...
└── If I play there...
    └── ... (and so on)
```

**Typical values:**
- EASY: 50-200 nodes
- MEDIUM: 100-500 nodes  
- HARD: 300-2000 nodes

---

### 3. Simulations (Monte Carlo playouts)
**What it is:** Number of random games the **Monte Carlo algorithm** played out to test moves

**Example:** `Simulations: 300` = Monte Carlo played 300 complete random games

**What it tells you:**
- Each simulation plays a move, then finishes the game randomly
- More simulations = better understanding of which moves lead to wins

**How it works:**
For each candidate move, AI does this:
1. Place the move on the board
2. Play the rest of the game randomly until someone wins
3. Record if AI won or lost
4. Undo all moves and try again
5. After many simulations, calculate win rate

**Difficulty settings:**
- EASY: 10 simulations per tested move
- MEDIUM: 20 simulations per tested move
- HARD: 30 simulations per tested move

Since AI tests 15 different moves, total simulations are:
- EASY: 15 × 10 = 150 total
- MEDIUM: 15 × 20 = 300 total
- HARD: 15 × 30 = 450 total

---

### 4. Win Rate (percentage)
**What it is:** Percentage of Monte Carlo simulations that the AI won

**Example:** `Win rate: 0.0%` = AI lost all simulations from that position (bad position!)

**What it tells you:**
- 100% = AI won every random playout (excellent move!)
- 50-75% = Pretty good move
- 25-50% = Okay move
- 0-25% = Bad position (AI is losing)

**Why it changes:**
- Early game: Often low (0-30%) because position is equal
- Mid game: Varies (20-60%) as advantage shifts
- Late game: High (70-100%) if AI is winning, low if losing

---

### 5. Score (evaluation)
**What it is:** Minimax algorithm's numerical evaluation of the position

**Example:** `Score: 3.5` = Position is slightly favorable

**What it tells you:**
- Positive score (1.0, 5.0, 10.0) = AI thinks it's winning
- Negative score (-2.0, -8.0) = AI thinks it's losing
- Zero (0.0) = Position is equal
- Very high (100+) = AI sees a winning path
- Very low (-100+) = AI sees it will lose

**How it's calculated:**
```
Score = (My connectivity - Opponent connectivity) × 5.0
      + (My bridges - Opponent bridges) × 2.0  
      + (My stones - Opponent stones) × 0.5
```

**Terms:**
- **Connectivity**: How close AI is to connecting its edges
- **Bridges**: Pairs of stones that support each other
- **Stones**: Number of pieces on the board

---

## How the AI Combines Both Algorithms

The AI uses **both** Minimax AND Monte Carlo together:

```
1. Minimax finds best move with strategic analysis
2. Monte Carlo finds best move with random simulations
3. AI compares both results:
   - If Monte Carlo win rate > 75%: Use Monte Carlo's move
   - Otherwise: Use Minimax's move
```

This hybrid approach combines:
- **Minimax strengths**: Strategic thinking, tactical analysis
- **Monte Carlo strengths**: Long-term winning probability

---

## Example Interpretation

Let's say you see:
```
Think time: 125 ms
Nodes: 420
Simulations: 300
Win rate: 68.5%
Score: 7.2
```

**What this means:**
- AI took 125ms (fast, good response time)
- Looked at 420 positions (decent search depth)
- Played 300 random games to test moves
- Won 68.5% of those games (strong position!)
- Minimax evaluates position as +7.2 (AI is winning)

**Conclusion:** AI found a good move and is in a strong position!

---

## Difficulty Levels Comparison

| Difficulty | Think Time | Nodes | Simulations | Quality |
|-----------|-----------|-------|-------------|---------|
| EASY | 20-50 ms | 50-200 | 150 | Makes mistakes, good for beginners |
| MEDIUM | 50-150 ms | 100-500 | 300 | Balanced, challenging but beatable |
| HARD | 150-500 ms | 300-2000 | 450 | Strong player, hard to beat |

---

## Why These Statistics Are Correct

1. **Think time**: Measured with C++ high-resolution clock (accurate to microseconds)
2. **Nodes**: Counted during Minimax tree traversal (every position explored)
3. **Simulations**: Controlled by difficulty setting, counted explicitly
4. **Win rate**: Calculated as `(wins / simulations) × 100%`
5. **Score**: Computed from board evaluation function with connectivity heuristics

All values are directly measured from the actual algorithms running!

---

## For Your Teacher/Demo

You can explain:
- "The AI uses TWO algorithms working together"
- "Minimax does strategic analysis, Monte Carlo does probability testing"
- "Higher difficulty = more thinking time, more nodes, more simulations"
- "Statistics prove the AI is actually calculating, not just random moves"
- "Think time shows real-time performance optimization"

This demonstrates understanding of:
✅ Game tree search (Minimax)
✅ Probabilistic methods (Monte Carlo)
✅ Algorithm hybridization
✅ Performance optimization
✅ Heuristic evaluation functions
