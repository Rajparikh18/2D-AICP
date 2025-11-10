#pragma once
#include "HexGrid.h"
#include "Minimax.h"
#include "MonteCarlo.h"

struct MoveInfo {
    Move move;
    double score;
    int nodesEvaluated;
    int simulations;
    double winRate;
    int thinkTime;
    bool isWinningMove;
    bool isBlockingMove;
};

class AI {
public:
    AI();
    
    MoveInfo calculateMove(HexGrid& grid);
    
private:
    Minimax minimax;
    MonteCarlo monteCarlo;
    
    // Critical move detection
    HexCoord findImmediateWin(HexGrid& grid, Player player);
    HexCoord findImmediateBlock(HexGrid& grid, Player player);
    std::vector<HexCoord> findCriticalCells(HexGrid& grid, Player player);
};
