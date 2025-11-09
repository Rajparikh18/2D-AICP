#include "PathFinding.h"
#include <unordered_map>
#include <limits>

bool PathFinding::hasWinningPath(const HexGrid& grid, Player player) {
    std::vector<HexCoord> startEdge, goalEdge;
    
    if (player == Player::RED) {
        startEdge = grid.getTopEdge();
        goalEdge = grid.getBottomEdge();
    } else {
        startEdge = grid.getLeftEdge();
        goalEdge = grid.getRightEdge();
    }
    
    std::unordered_set<HexCoord> goalSet(goalEdge.begin(), goalEdge.end());
    std::unordered_set<HexCoord> visited;
    std::queue<HexCoord> queue;
    
    for (const HexCoord& coord : startEdge) {
        if (grid.getCell(coord) == player) {
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
        
        std::vector<HexCoord> neighbors = grid.getNeighbors(current);
        for (const HexCoord& neighbor : neighbors) {
            if (visited.find(neighbor) == visited.end() && 
                grid.getCell(neighbor) == player) {
                visited.insert(neighbor);
                queue.push(neighbor);
            }
        }
    }
    
    return false;
}

double PathFinding::calculateConnectivity(const HexGrid& grid, Player player) {
    std::vector<HexCoord> startEdge, goalEdge;
    
    if (player == Player::RED) {
        startEdge = grid.getTopEdge();
        goalEdge = grid.getBottomEdge();
    } else {
        startEdge = grid.getLeftEdge();
        goalEdge = grid.getRightEdge();
    }
    
    std::unordered_set<HexCoord> goalSet(goalEdge.begin(), goalEdge.end());
    double minDistance = std::numeric_limits<double>::max();
    
    for (const HexCoord& start : startEdge) {
        if (grid.getCell(start) != player) continue;
        
        std::unordered_map<HexCoord, int> distances;
        std::queue<HexCoord> queue;
        std::unordered_set<HexCoord> visited;
        
        distances[start] = 0;
        queue.push(start);
        visited.insert(start);
        
        while (!queue.empty()) {
            HexCoord current = queue.front();
            queue.pop();
            
            if (goalSet.find(current) != goalSet.end()) {
                minDistance = std::min(minDistance, (double)distances[current]);
                break;
            }
            
            std::vector<HexCoord> neighbors = grid.getNeighbors(current);
            for (const HexCoord& neighbor : neighbors) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    if (grid.getCell(neighbor) == player) {
                        distances[neighbor] = distances[current];
                    } else {
                        distances[neighbor] = distances[current] + 1;
                    }
                    queue.push(neighbor);
                }
            }
        }
    }
    
    if (minDistance == std::numeric_limits<double>::max()) {
        return 0.0;
    }
    
    return HexGrid::BOARD_SIZE - minDistance;
}

int PathFinding::countBridges(const HexGrid& grid, Player player) {
    int count = 0;
    
    for (const auto& kv : grid.getGrid()) {
        if (kv.second == player) {
            std::vector<HexCoord> neighbors = grid.getNeighbors(kv.first);
            int friendlyNeighbors = 0;
            
            for (const HexCoord& neighbor : neighbors) {
                if (grid.getCell(neighbor) == player) {
                    friendlyNeighbors++;
                }
            }
            
            if (friendlyNeighbors >= 2) {
                count++;
            }
        }
    }
    
    return count;
}
