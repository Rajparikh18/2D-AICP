#include "MonteCarlo.h"
#include <vector>
#include <algorithm>

MonteCarloResult MonteCarlo::findBestMove(HexGrid& grid, int simulations) {
    std::random_device rd;
    rng.seed(rd());
    
    Player player = grid.getCurrentPlayer();
    
    std::vector<HexCoord> emptyCells;
    for (const auto& kv : grid.getGrid()) {
        if (kv.second == Player::NONE) {
            emptyCells.push_back(kv.first);
        }
    }
    
    if (emptyCells.empty()) {
        return MonteCarloResult{Move(), 0.0, 0};
    }
    
    std::shuffle(emptyCells.begin(), emptyCells.end(), rng);
    int movesToTry = std::min(20, (int)emptyCells.size());  // Increased from 15
    
    Move bestMove;
    double bestWinRate = -1.0;
    int totalSimulations = 0;
    
    for (int i = 0; i < movesToTry; ++i) {
        const HexCoord& coord = emptyCells[i];
        int wins = 0;
        
        for (int sim = 0; sim < simulations; ++sim) {
            grid.makeMove(coord);
            Player result = simulatePlayout(grid, player);
            grid.undoMove();
            
            if (result == player) {
                wins++;
            }
        }
        
        double winRate = (double)wins / simulations;
        totalSimulations += simulations;
        
        if (winRate > bestWinRate) {
            bestWinRate = winRate;
            bestMove = Move(coord, player);
        }
    }
    
    return MonteCarloResult{bestMove, bestWinRate, totalSimulations};
}

Player MonteCarlo::simulatePlayout(HexGrid& grid, Player originalPlayer) {
    const int MAX_MOVES = 50;  // Reduced from 200
    int movesMade = 0;
    
    std::vector<HexCoord> emptyCells;
    
    Player winner = Player::NONE;
    
    // Check for winner every 3 moves instead of every move
    while (movesMade < MAX_MOVES) {
        if (movesMade % 3 == 0) {  // Only check winner every 3 moves
            winner = grid.getWinner();
            if (winner != Player::NONE) break;
        }
        
        emptyCells.clear();
        for (const auto& kv : grid.getGrid()) {
            if (kv.second == Player::NONE) {
                emptyCells.push_back(kv.first);
            }
        }
        
        if (emptyCells.empty()) {
            winner = grid.getWinner();  // Final check
            break;
        }
        
        std::uniform_int_distribution<int> dist(0, emptyCells.size() - 1);
        HexCoord randomCoord = emptyCells[dist(rng)];
        
        grid.makeMove(randomCoord);
        movesMade++;
    }
    
    // Final winner check if we exited due to MAX_MOVES
    if (winner == Player::NONE) {
        winner = grid.getWinner();
    }
    
    // Undo all moves
    for (int i = 0; i < movesMade; ++i) {
        grid.undoMove();
    }
    
    return winner;
}
