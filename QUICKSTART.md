# Quick Start Guide - Hex Game

## To Build and Run the Game:

### Method 1: Build and Run (Recommended)
```batch
build.bat
```
This will compile and automatically launch the game.

### Method 2: Just Run (if already built)
```batch
run.bat
```
This will launch the game if it's already compiled.

### Method 3: Manual Commands
```powershell
# Build
g++ -std=c++14 -O2 -Wall -o build\HexGame.exe main.cpp HexGrid.cpp PathFinding.cpp Minimax.cpp MonteCarlo.cpp AI.cpp -lgdi32 -luser32 -lkernel32 -mwindows

# Run
start build\HexGame.exe
```

## If You Get "Permission Denied" Error:

1. **Close the game window first** (if it's already running)
2. Try again - Windows won't rebuild if the .exe is running
3. Or run: `run.bat` instead to just launch it

## Game Controls:

- **Click** - Place your RED stone
- **N** - New game
- **Ctrl+Z** - Undo move  
- **1** - Easy difficulty
- **2** - Medium difficulty
- **3** - Hard difficulty

## How to Play:

- You are RED - connect TOP to BOTTOM
- AI is BLUE - connects LEFT to RIGHT
- Click on any empty hexagon to place your stone
- AI will automatically play after you

## Features:

✅ All 4 AI Algorithms working:
1. Minimax with Alpha-Beta Pruning
2. Monte Carlo Tree Search
3. BFS Pathfinding
4. A* Heuristic Evaluation

✅ Real-time AI statistics displayed
✅ Beautiful hexagonal board
✅ Hover effects
✅ Professional Windows GUI

## Troubleshooting:

**"Permission Denied" when building:**
- Close the game window first
- The .exe file is locked while running

**Game doesn't start:**
- Make sure build.bat completed successfully
- Check that `build\HexGame.exe` exists

**Can't see the window:**
- Check your taskbar
- Press Alt+Tab to switch windows

---

**Enjoy the game!** 🎮
