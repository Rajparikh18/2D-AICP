#include "AI.h"
#include <chrono>

AI::AI(Difficulty difficulty) : difficulty(difficulty) {}

void AI::setDifficulty(Difficulty difficulty) {
    this->difficulty = difficulty;
}

MoveInfo AI::calculateMove(HexGrid& grid) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    int depth = 2;
    int sims = 100;
    
    switch (difficulty) {
        case Difficulty::EASY:
            depth = 2;
            sims = 30;
            break;
        case Difficulty::MEDIUM:
            depth = 2;
            sims = 100;
            break;
        case Difficulty::HARD:
            depth = 3;
            sims = 200;
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
