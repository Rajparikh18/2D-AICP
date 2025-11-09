# Hex Game Rules & Win Conditions 🎯

## Game Objective

**RED (You):** Connect the TOP edge to the BOTTOM edge
**BLUE (AI):** Connect the LEFT edge to the RIGHT edge

## Board Layout

```
        TOP (RED wins by reaching here)
         ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○
        ○ · · · · · · · · · · ○
LEFT   ○ · · · · · · · · · · · ○   RIGHT
(BLUE  ○ · · · · · · · · · · · ○   (BLUE
wins)  ○ · · · · · · · · · · · ○   wins)
       ○ · · · · · · · · · · · ○
        ○ · · · · · · · · · · ○
         ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○
        BOTTOM (RED wins by reaching here)
```

The circles (○) on the edges show the winning goals:
- **Red circles** = TOP and BOTTOM edges (RED's goal)
- **Blue circles** = LEFT and RIGHT edges (BLUE's goal)

## How Hex Cells Connect

In Hex, stones are **neighbors** if they touch at an edge (not just corners):

```
     ╱ ╲
    ╱ A ╲___╱ ╲
    ╲___╱ B ╲___╱
    ╱ C ╲___╱ D ╲
    ╲___╱   ╲___╱
```

- A connects to: B, C
- B connects to: A, C, D
- C connects to: A, B
- D connects to: B

**NOT connected:** A and D (they only touch at a corner)

## Win Condition Details

### RED Wins When:
You create an **unbroken chain** of RED stones from **any cell on the top edge** to **any cell on the bottom edge**.

Example RED winning path:
```
TOP:    [R] ○ ○ ○ ○ ○ ○ ○ ○ ○ ○
         · [R] · · · · · · · · ○
        ○ · [R] · · · · · · · · ○
        ○ · · [R][R] · · · · · · ○
        ○ · · · · [R] · · · · · ○
        ○ · · · · · [R][R] · · · ○
        ○ · · · · · · · [R] · · ○
         ○ ○ ○ ○ ○ ○ ○ ○ [R] ○ ○
BOTTOM:  (Each [R] connects to the next)
```

### BLUE Wins When:
AI creates an **unbroken chain** of BLUE stones from **any cell on the left edge** to **any cell on the right edge**.

Example BLUE winning path:
```
        ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○
LEFT:  [B] · · · · · · · · · · ○
       [B][B] · · · · · · · · · ○  RIGHT
        ○ · [B][B] · · · · · · [B]  ←
        ○ · · · [B][B] · · · · [B]  ←
        ○ · · · · · [B][B][B][B] ○  ←
        ○ · · · · · · · · · · ○
         ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○
```

## Important Hex Rules

### 1. No Draws Possible
- In Hex, someone MUST win eventually
- It's mathematically impossible to have a draw
- One player will always complete their connection first

### 2. First Player Advantage
- The player who goes first (RED/you) has a theoretical advantage
- That's why the AI tries extra hard! 😊

### 3. Blocking Strategy
- You can block AI's connection by cutting across its path
- AI will try to block your vertical path
- Strategy involves both offense (building your path) and defense (blocking opponent)

## How the Game Detects Wins

The game uses a **BFS (Breadth-First Search) algorithm**:

### For RED:
1. Start from all RED stones on the TOP edge
2. Check each stone's 6 neighbors
3. If neighbor is also RED, add it to search
4. Continue until finding a RED stone on BOTTOM edge
5. If found → RED WINS!

### For BLUE:
1. Start from all BLUE stones on the LEFT edge
2. Check each stone's 6 neighbors  
3. If neighbor is also BLUE, add it to search
4. Continue until finding a BLUE stone on RIGHT edge
5. If found → BLUE WINS!

## Common Win Detection Issues

### "Why didn't I win? I have stones from top to bottom!"

Check these:
1. ✓ Are ALL stones actually RED?
2. ✓ Does EVERY stone touch the NEXT stone at an edge?
3. ✓ No gaps or diagonal-only connections?
4. ✓ Path starts from TOP edge (r=0)?
5. ✓ Path ends at BOTTOM edge (r=10)?

### Visual Check
If you think you won, trace your finger along the path:
- Can you move from stone to stone without lifting your finger?
- Do you start at the top red circle border?
- Do you end at the bottom red circle border?

If YES to all → You should have won! (Report this as a bug)

## Strategy Tips

### For Beginners (Against EASY AI):
1. **Pick a side:** Choose left or right side to build your path
2. **Build vertically:** Try to create a straight-ish path downward
3. **Don't ignore AI:** Block BLUE if it's getting close to connecting
4. **Use corners:** Corner hexes are hard for AI to block

### Against MEDIUM AI:
1. **Create threats:** Make multiple possible paths
2. **Block actively:** Don't let BLUE build long chains
3. **Connect groups:** Join separate clusters of RED stones
4. **Watch bridges:** Two stones can "virtually" connect (bridge pattern)

### Against HARD AI:
1. **Think ahead:** Plan 3-4 moves in advance
2. **Efficient paths:** Use shortest path possible
3. **Force moves:** Create situations where AI MUST respond
4. **Minimize AI options:** Play moves that limit AI's choices

## Game Controls

- **Click hex:** Place your RED stone
- **N key:** Start a new game
- **Ctrl+Z:** Undo last move (yours AND AI's)
- **1 key:** Switch to EASY difficulty
- **2 key:** Switch to MEDIUM difficulty  
- **3 key:** Switch to HARD difficulty

## Testing Win Detection

Want to test if win detection works? Try this:

**Quick RED win test:**
1. Start new game (N key)
2. Click straight down the middle column:
   - (5, 0) - top
   - (5, 1)
   - (5, 2)
   - (5, 3)
   - ... continue to ...
   - (5, 10) - bottom
3. You should win before reaching bottom (AI will block you)

**Why it works:** Creates direct vertical path from top to bottom

## For Your Project Demo

When explaining to your teacher:

✅ **"Hex is a connection game where each player tries to connect opposite sides"**

✅ **"No draws are possible - proven mathematically by John Nash"**

✅ **"Win detection uses BFS graph traversal to find connecting paths"**

✅ **"The game checks for winners after EVERY move, so wins are detected immediately"**

✅ **"Edge highlighting shows each player's goal - RED connects red circles, BLUE connects blue circles"**

This shows you understand:
- Game theory
- Graph algorithms (BFS)
- Path-finding in connected networks
- UI/UX design for game clarity
