#pragma once
#include "HexGrid.h"
#include "PathFinding.h"
#include <algorithm>
#include <limits>

struct MinimaxResult {
    Move move;
    double score;
    int nodesEvaluated;
};

class Minimax {
public:
    MinimaxResult findBestMove(HexGrid& grid, int depth);
    
private:
    int nodesEvaluated;
    
    double minimaxAlphaBeta(HexGrid& grid, int depth, double alpha, double beta, Player originalPlayer);
    double evaluatePosition(const HexGrid& grid, Player player);
};
