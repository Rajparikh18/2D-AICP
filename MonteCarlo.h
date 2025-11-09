#pragma once
#include "HexGrid.h"
#include <random>

struct MonteCarloResult {
    Move move;
    double winRate;
    int simulations;
};

class MonteCarlo {
public:
    MonteCarloResult findBestMove(HexGrid& grid, int simulations);
    
private:
    std::mt19937 rng;
    
    Player simulatePlayout(HexGrid& grid, Player originalPlayer);
};
