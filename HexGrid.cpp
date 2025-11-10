#include "HexGrid.h"
#include <queue>
#include <unordered_set>

const HexCoord HexGrid::DIRECTIONS[6] = {
    HexCoord(1, 0), HexCoord(1, -1), HexCoord(0, -1),
    HexCoord(-1, 0), HexCoord(-1, 1), HexCoord(0, 1)
};

HexGrid::HexGrid() : currentPlayer(Player::RED) {
    reset();
}

void HexGrid::reset() {
    grid.clear();
    for (int q = 0; q < BOARD_SIZE; ++q) {
        for (int r = 0; r < BOARD_SIZE; ++r) {
            grid[HexCoord(q, r)] = Player::NONE;
        }
    }
    currentPlayer = Player::RED;
    moveHistory.clear();
}

Player HexGrid::getCell(const HexCoord& coord) const {
    auto it = grid.find(coord);
    return (it != grid.end()) ? it->second : Player::NONE;
}

bool HexGrid::makeMove(const HexCoord& coord) {
    auto it = grid.find(coord);
    if (it != grid.end() && it->second == Player::NONE) {
        Move move(coord, currentPlayer);
        grid[coord] = currentPlayer;
        moveHistory.push_back(move);
        currentPlayer = (currentPlayer == Player::RED) ? Player::BLUE : Player::RED;
        return true;
    }
    return false;
}

// Place a move explicitly for the given player (used ONLY for simulation - doesn't change turn)
bool HexGrid::makeMoveFor(const HexCoord& coord, Player player) {
    auto it = grid.find(coord);
    if (it != grid.end() && it->second == Player::NONE) {
        Move move(coord, player);
        grid[coord] = player;
        moveHistory.push_back(move);
        // DON'T change currentPlayer - this is for simulation only!
        // The calling code will handle turn management properly
        return true;
    }
    return false;
}

// Simulate a move without affecting move history (safer for AI evaluation)
bool HexGrid::simulateMove(const HexCoord& coord, Player player) {
    auto it = grid.find(coord);
    if (it != grid.end() && it->second == Player::NONE) {
        grid[coord] = player;
        return true;
    }
    return false;
}

void HexGrid::undoSimulation(const HexCoord& coord) {
    grid[coord] = Player::NONE;
}

void HexGrid::undoMove() {
    if (!moveHistory.empty()) {
        Move lastMove = moveHistory.back();
        moveHistory.pop_back();
        grid[lastMove.coord] = Player::NONE;
        currentPlayer = lastMove.player;
    }
}

std::vector<HexCoord> HexGrid::getNeighbors(const HexCoord& coord) const {
    std::vector<HexCoord> neighbors;
    for (int i = 0; i < 6; ++i) {
        HexCoord neighbor(coord.q + DIRECTIONS[i].q, coord.r + DIRECTIONS[i].r);
        if (grid.find(neighbor) != grid.end()) {
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

std::vector<HexCoord> HexGrid::getTopEdge() const {
    std::vector<HexCoord> edge;
    for (int q = 0; q < BOARD_SIZE; ++q) {
        edge.push_back(HexCoord(q, 0));
    }
    return edge;
}

std::vector<HexCoord> HexGrid::getBottomEdge() const {
    std::vector<HexCoord> edge;
    for (int q = 0; q < BOARD_SIZE; ++q) {
        edge.push_back(HexCoord(q, BOARD_SIZE - 1));
    }
    return edge;
}

std::vector<HexCoord> HexGrid::getLeftEdge() const {
    std::vector<HexCoord> edge;
    for (int r = 0; r < BOARD_SIZE; ++r) {
        edge.push_back(HexCoord(0, r));
    }
    return edge;
}

std::vector<HexCoord> HexGrid::getRightEdge() const {
    std::vector<HexCoord> edge;
    for (int r = 0; r < BOARD_SIZE; ++r) {
        edge.push_back(HexCoord(BOARD_SIZE - 1, r));
    }
    return edge;
}

Player HexGrid::getWinner() const {
    if (hasWinningPath(Player::RED, getTopEdge(), getBottomEdge())) {
        return Player::RED;
    }
    if (hasWinningPath(Player::BLUE, getLeftEdge(), getRightEdge())) {
        return Player::BLUE;
    }
    return Player::NONE;
}

bool HexGrid::hasWinningPath(Player player,
                             const std::vector<HexCoord>& startEdge,
                             const std::vector<HexCoord>& goalEdge) const {
    std::unordered_set<HexCoord> goalSet(goalEdge.begin(), goalEdge.end());
    std::unordered_set<HexCoord> visited;
    std::queue<HexCoord> queue;
    
    for (const HexCoord& coord : startEdge) {
        if (getCell(coord) == player) {
            queue.push(coord);
            visited.insert(coord);
        }
    }
    
    while (!queue.empty()) {
        HexCoord current = queue.front();
        queue.pop();
        
        if (goalSet.find(current) != goalSet.end()) {
            return true;
        }
        
        std::vector<HexCoord> neighbors = getNeighbors(current);
        for (const HexCoord& neighbor : neighbors) {
            if (visited.find(neighbor) == visited.end() && 
                getCell(neighbor) == player) {
                visited.insert(neighbor);
                queue.push(neighbor);
            }
        }
    }
    
    return false;
}
