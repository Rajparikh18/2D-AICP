#pragma once
#include "HexCoord.h"
#include <unordered_map>
#include <vector>

class HexGrid {
public:
    static const int BOARD_SIZE = 11;
    
    HexGrid();
    void reset();
    
    Player getCell(const HexCoord& coord) const;
    bool makeMove(const HexCoord& coord);
    void undoMove();
    
    Player getCurrentPlayer() const { return currentPlayer; }
    Player getWinner() const;
    
    std::vector<HexCoord> getNeighbors(const HexCoord& coord) const;
    std::vector<HexCoord> getTopEdge() const;
    std::vector<HexCoord> getBottomEdge() const;
    std::vector<HexCoord> getLeftEdge() const;
    std::vector<HexCoord> getRightEdge() const;
    
    const std::unordered_map<HexCoord, Player>& getGrid() const { return grid; }
    const std::vector<Move>& getMoveHistory() const { return moveHistory; }
    
    // Place a move for a specific player (used for safe simulation)
    bool makeMoveFor(const HexCoord& coord, Player player);
    
    // Simulate a move without affecting move history (for AI evaluation)
    bool simulateMove(const HexCoord& coord, Player player);
    void undoSimulation(const HexCoord& coord);
    
private:
    std::unordered_map<HexCoord, Player> grid;
    Player currentPlayer;
    std::vector<Move> moveHistory;
    
    static const HexCoord DIRECTIONS[6];
    
    bool hasWinningPath(Player player, 
                       const std::vector<HexCoord>& startEdge,
                       const std::vector<HexCoord>& goalEdge) const;
};
