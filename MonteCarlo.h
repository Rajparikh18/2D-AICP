#pragma once
#include "HexGrid.h"
#include "PathFinding.h"
#include <random>
#include <vector>

struct MonteCarloResult {
    Move move;
    double winRate;
    int simulations;
};

namespace MonteCarloInternal {
    struct MoveScore {
        HexCoord coord;
        double score;
        
        bool operator<(const MoveScore& other) const {
            return score > other.score; // Higher scores first
        }
    };
}

class MonteCarlo {
public:
    MonteCarloResult findBestMove(HexGrid& grid, int simulations);
    
private:
    std::mt19937 rng;
    
    Player simulatePlayout(HexGrid& grid, Player originalPlayer);
    double scoreMoveHeuristic(HexGrid& grid, const HexCoord& move, Player player);
    std::vector<HexCoord> orderMovesByHeuristic(HexGrid& grid, const std::vector<HexCoord>& moves, Player player);
};
