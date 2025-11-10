#pragma once
#include "HexGrid.h"
#include "PathFinding.h"
#include <algorithm>
#include <limits>
#include <vector>

struct MinimaxResult {
    Move move;
    double score;
    int nodesEvaluated;
};

namespace MinimaxInternal {
    struct MoveScore {
        HexCoord coord;
        double score;
        
        bool operator<(const MoveScore& other) const {
            return score > other.score; // Higher scores first
        }
    };
}

class Minimax {
public:
    MinimaxResult findBestMove(HexGrid& grid, int depth);
    
private:
    int nodesEvaluated;
    
    double minimaxAlphaBeta(HexGrid& grid, int depth, double alpha, double beta, Player originalPlayer);
    double evaluatePosition(const HexGrid& grid, Player player);
    double scoreMoveHeuristic(HexGrid& grid, const HexCoord& move, Player player);
    std::vector<HexCoord> orderMovesByHeuristic(HexGrid& grid, const std::vector<HexCoord>& moves, Player player);
};
