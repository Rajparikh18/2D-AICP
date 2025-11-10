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
    
    // Sort moves by heuristic instead of random shuffle
    emptyCells = orderMovesByHeuristic(grid, emptyCells, player);
    
    int movesToTry = std::min(8, (int)emptyCells.size()); // Further reduced for speed
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

double MonteCarlo::scoreMoveHeuristic(HexGrid& grid, const HexCoord& move, Player player) {
    Player opponent = (player == Player::RED) ? Player::BLUE : Player::RED;
    
    double score = 0.0;
    
    // 1. Check immediate win/block
    grid.makeMoveFor(move, player);
    Player winner = grid.getWinner();
    if (winner == player) {
        grid.undoMove();
        return 100000.0; // Winning move!
    }
    grid.undoMove();
    
    // 2. Calculate connectivity BEFORE the move
    double myConnectivityBefore = PathFinding::calculateConnectivity(grid, player);
    double oppConnectivityBefore = PathFinding::calculateConnectivity(grid, opponent);
    
    // 3. Make the move and calculate connectivity AFTER
    grid.makeMoveFor(move, player);
    double myConnectivityAfter = PathFinding::calculateConnectivity(grid, player);
    double oppConnectivityAfter = PathFinding::calculateConnectivity(grid, opponent);
    grid.undoMove();
    
    // 4. Calculate improvements/blocking
    double myImprovement = myConnectivityAfter - myConnectivityBefore;
    double oppReduction = oppConnectivityBefore - oppConnectivityAfter; // Positive = we blocked them
    
    // 5. ULTRA-AGGRESSIVE weighting based on game state
    double myWeight = 100.0;
    double oppWeight = 200.0;  // Defense MORE important by default!
    
    // If opponent is ahead, prioritize blocking MUCH MORE!
    if (oppConnectivityBefore > myConnectivityBefore + 1.0) {
        oppWeight = 400.0; // Super aggressive blocking when behind
    }
    
    // If opponent is very close to winning, EMERGENCY BLOCKING!
    if (oppConnectivityBefore > HexGrid::BOARD_SIZE * 1.4) {
        oppWeight = 800.0; // CRITICAL blocking
        score += oppReduction * 300.0; // Massive bonus for blocking
    } else if (oppConnectivityBefore > HexGrid::BOARD_SIZE * 1.0) {
        oppWeight = 500.0; // High priority blocking
        score += oppReduction * 150.0;
    }
    
    score += myImprovement * myWeight;
    score += oppReduction * oppWeight;
    
    // 6. Check if this blocks an immediate opponent win
    int oppNeighbors = 0;
    std::vector<HexCoord> neighbors = grid.getNeighbors(move);
    for (const HexCoord& n : neighbors) {
        if (grid.getCell(n) == opponent) {
            oppNeighbors++;
        }
    }
    if (oppNeighbors >= 2) {
        score += 3000.0; // Blocking a strong opponent connection
    }
    
    // 7. Count friendly neighbors (encourage connection)
    int friendlyNeighbors = 0;
    for (const HexCoord& neighbor : neighbors) {
        if (grid.getCell(neighbor) == player) {
            friendlyNeighbors++;
        }
    }
    score += friendlyNeighbors * 80.0;  // Bonus for connecting to our stones
    
    // 8. Strategic positioning - prefer center of the board
    int centerQ = HexGrid::BOARD_SIZE / 2;
    int centerR = HexGrid::BOARD_SIZE / 2;
    int distToCenter = abs(move.q - centerQ) + abs(move.r - centerR);
    score += (HexGrid::BOARD_SIZE - distToCenter) * 5.0;
    
    return score;
}

std::vector<HexCoord> MonteCarlo::orderMovesByHeuristic(HexGrid& grid, const std::vector<HexCoord>& moves, Player player) {
    std::vector<MonteCarloInternal::MoveScore> scoredMoves;
    scoredMoves.reserve(moves.size());
    
    Player opponent = (player == Player::RED) ? Player::BLUE : Player::RED;
    
    // Calculate baseline connectivity ONCE
    double myConnBefore = PathFinding::calculateConnectivity(grid, player);
    double oppConnBefore = PathFinding::calculateConnectivity(grid, opponent);
    
    // Score EVERY move by actual connectivity impact
    for (const HexCoord& move : moves) {
        // score as if 'player' played the move
        grid.makeMoveFor(move, player);
        
        // Check immediate win
        if (grid.getWinner() == player) {
            scoredMoves.push_back({move, 1000000.0});
            grid.undoMove();
            continue;
        }
        
        // Calculate connectivity after move
        double myConnAfter = PathFinding::calculateConnectivity(grid, player);
        double oppConnAfter = PathFinding::calculateConnectivity(grid, opponent);
        
        grid.undoMove();
        
        // Calculate impact
        double myGain = myConnAfter - myConnBefore;
        double oppLoss = oppConnBefore - oppConnAfter;
        
        // ULTRA-AGGRESSIVE Adaptive weights (matching Minimax)
        double offenseWeight = 1.0;
        double defenseWeight = 2.0;  // Default: defense is MORE important
        
        if (oppConnBefore > myConnBefore + 2.0) {
            defenseWeight = 5.0;
            offenseWeight = 0.3;
        } else if (oppConnBefore > myConnBefore + 1.0) {
            defenseWeight = 3.5;
            offenseWeight = 0.6;
        } else if (myConnBefore > oppConnBefore + 3.0) {
            offenseWeight = 2.5;
            defenseWeight = 1.0;
        } else if (oppConnBefore > HexGrid::BOARD_SIZE * 1.5) {
            defenseWeight = 8.0;
            offenseWeight = 0.1;
        } else if (oppConnBefore > HexGrid::BOARD_SIZE * 1.2) {
            defenseWeight = 6.0;
            offenseWeight = 0.2;
        }
        
        double score = (myGain * offenseWeight) + (oppLoss * defenseWeight);
        
        // Tie-breaker
        int friendlyNeighbors = 0;
        std::vector<HexCoord> neighbors = grid.getNeighbors(move);
        for (const HexCoord& n : neighbors) {
            if (grid.getCell(n) == player) {
                friendlyNeighbors++;
            }
        }
        score += friendlyNeighbors * 0.5;
        
        scoredMoves.push_back({move, score});
    }
    
    std::sort(scoredMoves.begin(), scoredMoves.end());
    
    std::vector<HexCoord> orderedMoves;
    orderedMoves.reserve(moves.size());
    for (const MonteCarloInternal::MoveScore& ms : scoredMoves) {
        orderedMoves.push_back(ms.coord);
    }
    
    return orderedMoves;
}
