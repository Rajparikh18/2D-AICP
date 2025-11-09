#include "AI.h"
#include <chrono>

AI::AI(Difficulty difficulty) : difficulty(difficulty) {}

void AI::setDifficulty(Difficulty difficulty) {
    this->difficulty = difficulty;
}

MoveInfo AI::calculateMove(HexGrid& grid) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    int depth = 2;
    int sims = 50;
    
    switch (difficulty) {
        case Difficulty::EASY:
            depth = 2;
            sims = 15;  // Slightly increased from 10
            break;
        case Difficulty::MEDIUM:
            depth = 3;  // Increased from 2 to look further ahead!
            sims = 25;  // Slightly increased from 20
            break;
        case Difficulty::HARD:
            depth = 4;  // Increased from 3 for stronger play!
            sims = 40;  // Increased from 30
            break;
    }
    
    MinimaxResult minimaxResult = minimax.findBestMove(grid, depth);
    MonteCarloResult mcResult = monteCarlo.findBestMove(grid, sims);
    
    Move finalMove = (mcResult.winRate > 0.75) ? mcResult.move : minimaxResult.move;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    int thinkTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    return MoveInfo{
        finalMove,
        minimaxResult.score,
        minimaxResult.nodesEvaluated,
        mcResult.simulations,
        mcResult.winRate,
        thinkTime
    };
}
