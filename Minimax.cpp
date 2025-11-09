#include "Minimax.h"
#include <vector>
#include <algorithm>
#include <random>

MinimaxResult Minimax::findBestMove(HexGrid& grid, int depth) {
    nodesEvaluated = 0;
    Player player = grid.getCurrentPlayer();
    
    std::vector<HexCoord> emptyCells;
    for (const auto& kv : grid.getGrid()) {
        if (kv.second == Player::NONE) {
            emptyCells.push_back(kv.first);
        }
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(emptyCells.begin(), emptyCells.end(), gen);
    
    Move bestMove;
    double bestScore = -std::numeric_limits<double>::max();
    double alpha = -std::numeric_limits<double>::max();
    double beta = std::numeric_limits<double>::max();
    
    int movesToCheck = std::min(35, (int)emptyCells.size());  // Increased from 30
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
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(emptyCells.begin(), emptyCells.end(), gen);
    
    double maxScore = -std::numeric_limits<double>::max();
    int movesToCheck = std::min(25, (int)emptyCells.size());  // Increased from 20
    
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
    
    // DEFENSIVE BOOST: Heavily prioritize blocking opponent!
    double score = (myConnectivity - oppConnectivity * 1.5) * 5.0;  // Opponent connectivity matters MORE
    score += (myBridges - oppBridges) * 2.0;
    score += (myStones - oppStones) * 0.5;
    
    // CRITICAL: If opponent is very close to winning, strongly penalize!
    if (oppConnectivity > HexGrid::BOARD_SIZE * 0.7) {  // Opponent 70% connected
        score -= 50.0;  // Big penalty!
    }
    
    return score;
}
