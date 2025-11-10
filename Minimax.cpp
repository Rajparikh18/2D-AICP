#include "Minimax.h"
#include <vector>
#include <algorithm>

MinimaxResult Minimax::findBestMove(HexGrid& grid, int depth) {
    nodesEvaluated = 0;
    Player player = grid.getCurrentPlayer();
    
    std::vector<HexCoord> emptyCells;
    for (const auto& kv : grid.getGrid()) {
        if (kv.second == Player::NONE) {
            emptyCells.push_back(kv.first);
        }
    }
    
    // Sort moves by heuristic score instead of random shuffle
    emptyCells = orderMovesByHeuristic(grid, emptyCells, player);
    
    Move bestMove;
    double bestScore = -std::numeric_limits<double>::max();
    double alpha = -std::numeric_limits<double>::max();
    double beta = std::numeric_limits<double>::max();
    
    // OPTIMIZED: Check fewer moves based on board state
    int moveCount = 0;
    for (const auto& kv : grid.getGrid()) {
        if (kv.second != Player::NONE) moveCount++;
    }
    
    int movesToCheck;
    if (moveCount < 8) {
        movesToCheck = std::min(12, (int)emptyCells.size()); // Early game: 12 moves
    } else if (moveCount > 40) {
        movesToCheck = std::min(20, (int)emptyCells.size()); // Late game: more moves
    } else {
        movesToCheck = std::min(15, (int)emptyCells.size()); // Mid game: 15 moves
    }
    for (int i = 0; i < movesToCheck; ++i) {
        const HexCoord& coord = emptyCells[i];
        
        grid.makeMove(coord);
        double score = -minimaxAlphaBeta(grid, depth - 1, -beta, -alpha, player);
        grid.undoMove();
        
        if (score > bestScore) {
            bestScore = score;
            bestMove = Move(coord, player);
        }
        
        alpha = std::max(alpha, score);
        if (alpha >= beta) break;
    }
    
    return MinimaxResult{bestMove, bestScore, nodesEvaluated};
}

double Minimax::minimaxAlphaBeta(HexGrid& grid, int depth, double alpha, double beta, Player originalPlayer) {
    nodesEvaluated++;
    
    Player winner = grid.getWinner();
    if (winner == originalPlayer) return 10000.0;
    if (winner != Player::NONE) return -10000.0;
    
    if (depth == 0) {
        return evaluatePosition(grid, originalPlayer);
    }
    
    std::vector<HexCoord> emptyCells;
    for (const auto& kv : grid.getGrid()) {
        if (kv.second == Player::NONE) {
            emptyCells.push_back(kv.first);
        }
    }
    
    if (emptyCells.empty()) return 0.0;
    
    // Sort moves by heuristic instead of random shuffle
    Player currentPlayer = grid.getCurrentPlayer();
    emptyCells = orderMovesByHeuristic(grid, emptyCells, currentPlayer);
    
    double maxScore = -std::numeric_limits<double>::max();
    int movesToCheck = std::min(10, (int)emptyCells.size()); // Further reduced for speed
    
    for (int i = 0; i < movesToCheck; ++i) {
        grid.makeMove(emptyCells[i]);
        double score = -minimaxAlphaBeta(grid, depth - 1, -beta, -alpha, originalPlayer);
        grid.undoMove();
        
        maxScore = std::max(maxScore, score);
        alpha = std::max(alpha, score);
        if (alpha >= beta) break;
    }
    
    return maxScore;
}

double Minimax::evaluatePosition(const HexGrid& grid, Player player) {
    Player opponent = (player == Player::RED) ? Player::BLUE : Player::RED;
    
    double myConnectivity = PathFinding::calculateConnectivity(grid, player);
    double oppConnectivity = PathFinding::calculateConnectivity(grid, opponent);
    
    int myBridges = PathFinding::countBridges(grid, player);
    int oppBridges = PathFinding::countBridges(grid, opponent);
    
    int myStones = 0, oppStones = 0;
    for (const auto& kv : grid.getGrid()) {
        if (kv.second == player) myStones++;
        else if (kv.second == opponent) oppStones++;
    }
    
    // AGGRESSIVE DEFENSE: Opponent's progress is MORE important than our progress!
    double score = 0.0;
    
    // Offensive score (our connectivity)
    score += myConnectivity * 8.0;
    score += myBridges * 3.0;
    score += myStones * 0.5;
    
    // DEFENSIVE SCORE (opponent's connectivity) - WEIGHTED HIGHER!
    score -= oppConnectivity * 12.0;  // Opponent connectivity hurts us MORE!
    score -= oppBridges * 5.0;
    score -= oppStones * 0.8;
    
    // CRITICAL THREAT DETECTION: If opponent is very close to winning
    if (oppConnectivity > HexGrid::BOARD_SIZE * 1.5) {
        score -= 100.0;  // MASSIVE penalty - opponent very close to winning!
    } else if (oppConnectivity > HexGrid::BOARD_SIZE * 1.2) {
        score -= 50.0;  // Big penalty - opponent making good progress
    }
    
    // Bonus for blocking opponent's path
    double connectivityGap = myConnectivity - oppConnectivity;
    if (connectivityGap > 0) {
        score += connectivityGap * 3.0;  // Bonus for being ahead
    } else {
        score += connectivityGap * 5.0;  // Extra penalty for being behind
    }
    
    return score;
}

double Minimax::scoreMoveHeuristic(HexGrid& grid, const HexCoord& move, Player player) {
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
    
    // ULTRA-AGGRESSIVE DEFENSIVE WEIGHTING
    double myWeight = 100.0;
    double oppWeight = 200.0;  // Defense is TWICE as important by default!
    
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

std::vector<HexCoord> Minimax::orderMovesByHeuristic(HexGrid& grid, const std::vector<HexCoord>& moves, Player player) {
    std::vector<MinimaxInternal::MoveScore> scoredMoves;
    scoredMoves.reserve(moves.size());
    
    Player opponent = (player == Player::RED) ? Player::BLUE : Player::RED;
    
    // Calculate baseline connectivity ONCE
    double myConnBefore = PathFinding::calculateConnectivity(grid, player);
    double oppConnBefore = PathFinding::calculateConnectivity(grid, opponent);
    
    // Score EVERY move by actual connectivity impact (this is the TRUE heuristic!)
    for (const HexCoord& move : moves) {
        // simulate move for the given player explicitly (safer)
        grid.makeMoveFor(move, player);
        
        // Check immediate win first
        if (grid.getWinner() == player) {
            scoredMoves.push_back({move, 1000000.0});
            grid.undoMove();
            continue;
        }
        
        // Calculate connectivity after this move
        double myConnAfter = PathFinding::calculateConnectivity(grid, player);
        double oppConnAfter = PathFinding::calculateConnectivity(grid, opponent);
        
        grid.undoMove();
        
        // Calculate the ACTUAL impact of this move
        double myGain = myConnAfter - myConnBefore;
        double oppLoss = oppConnBefore - oppConnAfter;  // Positive if we hurt opponent
        
        // Adaptive weights based on game state
        double offenseWeight = 1.0;
        double defenseWeight = 1.5;  // Slightly favor defense by default
        
        // ULTRA-AGGRESSIVE Dynamic strategy adjustment
        if (oppConnBefore > myConnBefore + 2.0) {
            // We're behind - MUST block aggressively
            defenseWeight = 5.0;
            offenseWeight = 0.3;
        } else if (oppConnBefore > myConnBefore + 1.0) {
            // Opponent slightly ahead - heavy defense
            defenseWeight = 3.5;
            offenseWeight = 0.6;
        } else if (myConnBefore > oppConnBefore + 3.0) {
            // We're well ahead - can focus on winning
            offenseWeight = 2.5;
            defenseWeight = 1.0;
        } else if (oppConnBefore > HexGrid::BOARD_SIZE * 1.5) {
            // Opponent VERY close to winning - EMERGENCY blocking!
            defenseWeight = 8.0;
            offenseWeight = 0.1;
        } else if (oppConnBefore > HexGrid::BOARD_SIZE * 1.2) {
            // Opponent close to winning - critical blocking!
            defenseWeight = 6.0;
            offenseWeight = 0.2;
        }
        
        // Final score is weighted sum of offense and defense
        double score = (myGain * offenseWeight) + (oppLoss * defenseWeight);
        
        // Small bonus for connecting to existing stones (tie-breaker)
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
    
    // Sort by score (highest first)
    std::sort(scoredMoves.begin(), scoredMoves.end());
    
    std::vector<HexCoord> orderedMoves;
    orderedMoves.reserve(moves.size());
    for (const MinimaxInternal::MoveScore& ms : scoredMoves) {
        orderedMoves.push_back(ms.coord);
    }
    
    return orderedMoves;
}
