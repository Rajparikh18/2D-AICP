@echo off
echo ========================================
echo Building Hex Game (C++ with Windows GUI)
echo ========================================
echo.

REM Create build directory
if not exist "build" mkdir build

echo Compiling...
g++ -std=c++14 -O2 -Wall ^
    -o build\HexGame.exe ^
    main.cpp ^
    HexGrid.cpp ^
    PathFinding.cpp ^
    Minimax.cpp ^
    MonteCarlo.cpp ^
    AI.cpp ^
    -lgdi32 -luser32 -lkernel32 -mwindows

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo BUILD FAILED!
    echo ========================================
    pause
    exit /b 1
)

echo.
echo ========================================
echo BUILD SUCCESSFUL!
echo ========================================
echo.
echo Executable: build\HexGame.exe
echo.
echo Running the game...
echo.

start build\HexGame.exe

pause
