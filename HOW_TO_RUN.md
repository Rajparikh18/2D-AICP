# How to Run the Hex Game

## Quick Start (Easiest Way)

### Method 1: Using the Batch File
1. Open Windows Explorer
2. Navigate to: `C:\College\AI-PROJECT\HexGame\`
3. Double-click `build.bat`
4. The game will compile and start automatically!

### Method 2: From Command Line
```bash
cd C:\College\AI-PROJECT\HexGame
build.bat
```

### Method 3: Run Pre-compiled Game
If already compiled, just run:
```bash
cd C:\College\AI-PROJECT\HexGame
build\HexGame.exe
```

## If You Make Code Changes

**IMPORTANT:** After changing any `.cpp` or `.h` files, you MUST rebuild:

1. Close the running game (if open)
2. Run `build.bat` again
3. The new version will start

## Troubleshooting

### "Permission Denied" Error
- The game is still running
- Solution: Close the game window first, then rebuild

### "g++ not recognized" Error
- MinGW is not in your PATH
- Solution: Make sure MinGW bin folder is in system PATH

### Game Shows Old Features
- You're running an old executable
- Solution: Delete `build\HexGame.exe` and rebuild with `build.bat`

## Game Files Structure

```
HexGame/
├── build.bat          ← RUN THIS to compile and start
├── build/
│   └── HexGame.exe    ← Compiled game
├── main.cpp           ← Main game code
├── HexGrid.cpp/h      ← Board logic
├── AI.cpp/h           ← AI controller
├── Minimax.cpp/h      ← Minimax algorithm
├── MonteCarlo.cpp/h   ← Monte Carlo simulation
└── PathFinding.cpp/h  ← Win detection & paths
```

## What Happens When You Run build.bat?

1. Creates `build/` folder (if doesn't exist)
2. Compiles all `.cpp` files with g++
3. Creates `HexGame.exe` in build folder
4. Automatically starts the game
5. Waits for you to press a key when done

That's it! Just run `build.bat` every time! 🎮
