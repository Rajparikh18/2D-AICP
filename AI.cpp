#include "AI.h"
#include "PathFinding.h"
#include <chrono>
#include <algorithm>

AI::AI() {}

MoveInfo AI::calculateMove(HexGrid& grid) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    Player aiPlayer = grid.getCurrentPlayer();
    Player opponent = (aiPlayer == Player::RED) ? Player::BLUE : Player::RED;
    
    bool isWinningMove = false;
    bool isBlockingMove = false;
    Move finalMove;
    
    // PRIORITY 1: Check if AI can win immediately
    HexCoord winMove = findImmediateWin(grid, aiPlayer);
    if (winMove.q != -1) {
        finalMove = Move(winMove, aiPlayer);
        isWinningMove = true;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        int thinkTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        return MoveInfo{
            finalMove,
            10000.0,
            1,
            0,
            1.0,
            thinkTime,
            isWinningMove,
            isBlockingMove
        };
    }
    
    // PRIORITY 2: Check if opponent can win next turn - MUST BLOCK!
    HexCoord blockMove = findImmediateBlock(grid, opponent);
    if (blockMove.q != -1) {
        finalMove = Move(blockMove, aiPlayer);
        isBlockingMove = true;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        int thinkTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        return MoveInfo{
            finalMove,
            5000.0,
            1,
            0,
            0.9,
            thinkTime,
            isWinningMove,
            isBlockingMove
        };
    }
    
    // PRIORITY 3: Use adaptive depth search with Minimax (primary) + Monte Carlo (validation)
    // Adaptive depth based on board state (less moves = deeper search possible)
    int moveCount = 0;
    for (const auto& kv : grid.getGrid()) {
        if (kv.second != Player::NONE) moveCount++;
    }
    
    int depth = 4;  // Balanced depth for speed + strength
    if (moveCount < 6) {
        depth = 3;  // Early game - many options, use shallower search
    } else if (moveCount > HexGrid::BOARD_SIZE * HexGrid::BOARD_SIZE - 15) {
        depth = 5;  // Endgame - fewer options, can search deeper
    }
    
    int sims = 30;  // Reduced from 50 for speed
    
    MinimaxResult minimaxResult = minimax.findBestMove(grid, depth);
    MonteCarloResult mcResult = monteCarlo.findBestMove(grid, sims);
    
    // Use Minimax as primary decision (it's better at tactics)
    // Only override if Monte Carlo has VERY high confidence AND disagrees
    if (mcResult.winRate > 0.85 && !(minimaxResult.move.coord == mcResult.move.coord)) {
        // Monte Carlo is very confident - consider its suggestion
        finalMove = mcResult.move;
    } else {
        // Trust Minimax (better at strategy and blocking)
        finalMove = minimaxResult.move;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    int thinkTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    return MoveInfo{
        finalMove,
        minimaxResult.score,
        minimaxResult.nodesEvaluated,
        mcResult.simulations,
        mcResult.winRate,
        thinkTime,
        isWinningMove,
        isBlockingMove
    };
}

HexCoord AI::findImmediateWin(HexGrid& grid, Player player) {
    // OPTIMIZED: Only check cells that are relevant (near player's stones or edges)
    std::vector<HexCoord> candidateCells;
    
    // Get player's edge cells
    std::vector<HexCoord> startEdge, goalEdge;
    if (player == Player::RED) {
        startEdge = grid.getTopEdge();
        goalEdge = grid.getBottomEdge();
    } else {
        startEdge = grid.getLeftEdge();
        goalEdge = grid.getRightEdge();
    }
    
    // Check cells near edges and near player's stones (much faster!)
    for (const auto& kv : grid.getGrid()) {
        if (kv.second == Player::NONE) {
            // Check if this empty cell is adjacent to player's stone
            bool isRelevant = false;
            std::vector<HexCoord> neighbors = grid.getNeighbors(kv.first);
            for (const HexCoord& n : neighbors) {
                if (grid.getCell(n) == player) {
                    isRelevant = true;
                    break;
                }
            }
            
            if (isRelevant) {
                candidateCells.push_back(kv.first);
            }
        }
    }
    
    // Quick check on only relevant cells
    for (const HexCoord& coord : candidateCells) {
        // Simulate AI move using the safe simulateMove method
        grid.simulateMove(coord, player);
        Player winner = grid.getWinner();
        grid.undoSimulation(coord);
        
        if (winner == player) {
            return coord;
        }
    }
    return HexCoord(-1, -1);
}

HexCoord AI::findImmediateBlock(HexGrid& grid, Player opponent) {
    // OPTIMIZED: Only check cells near opponent's stones (much faster!)
    std::vector<HexCoord> candidateCells;
    
    for (const auto& kv : grid.getGrid()) {
        if (kv.second == Player::NONE) {
            // Check if this empty cell is adjacent to opponent's stone
            bool isRelevant = false;
            std::vector<HexCoord> neighbors = grid.getNeighbors(kv.first);
            for (const HexCoord& n : neighbors) {
                if (grid.getCell(n) == opponent) {
                    isRelevant = true;
                    break;
                }
            }
            
            if (isRelevant) {
                candidateCells.push_back(kv.first);
            }
        }
    }
    
    // Quick check on only relevant cells  
    for (const HexCoord& coord : candidateCells) {
        // Simulate opponent move using the safe simulateMove method
        grid.simulateMove(coord, opponent);
        Player winner = grid.getWinner();
        grid.undoSimulation(coord);

        if (winner == opponent) {
            return coord;
        }
    }
    return HexCoord(-1, -1);
}

std::vector<HexCoord> AI::findCriticalCells(HexGrid& grid, Player player) {
    std::vector<HexCoord> criticalCells;
    
    // Find cells that significantly improve connectivity
    double baseConnectivity = PathFinding::calculateConnectivity(grid, player);
    
    for (const auto& kv : grid.getGrid()) {
        if (kv.second == Player::NONE) {
            grid.makeMove(kv.first);
            double newConnectivity = PathFinding::calculateConnectivity(grid, player);
            grid.undoMove();
            
            // If this move improves connectivity by a lot, it's critical
            if (newConnectivity - baseConnectivity > 2.0) {
                criticalCells.push_back(kv.first);
            }
        }
    }
    
    return criticalCells;
}
