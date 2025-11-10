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
    
    Player opponent = (player == Player::RED) ? Player::BLUE : Player::RED;
    std::unordered_set<HexCoord> goalSet(goalEdge.begin(), goalEdge.end());
    
    // Single BFS from ALL start positions at once (much faster!)
    std::unordered_map<HexCoord, int> distances;
    std::queue<HexCoord> queue;
    std::unordered_set<HexCoord> visited;
    
    // Initialize: add all valid start edge cells to queue
    for (const HexCoord& start : startEdge) {
        if (grid.getCell(start) == opponent) continue; // Opponent blocks this start
        
        int startCost = (grid.getCell(start) == player) ? 0 : 1;
        distances[start] = startCost;
        queue.push(start);
        visited.insert(start);
    }
    
    // If no valid starting positions, return very low score
    if (queue.empty()) {
        return -100.0;
    }
    
    int minDistanceToGoal = std::numeric_limits<int>::max();
    
    // BFS to find shortest path to goal
    while (!queue.empty()) {
        HexCoord current = queue.front();
        queue.pop();
        
        int currentDist = distances[current];
        
        // Early termination if we already found a better path
        if (currentDist >= minDistanceToGoal) {
            continue;
        }
        
        // Check if we reached goal
        if (goalSet.find(current) != goalSet.end()) {
            minDistanceToGoal = std::min(minDistanceToGoal, currentDist);
            continue;
        }
        
        // Explore neighbors
        std::vector<HexCoord> neighbors = grid.getNeighbors(current);
        for (const HexCoord& neighbor : neighbors) {
            // Skip opponent's stones
            if (grid.getCell(neighbor) == opponent) {
                continue;
            }
            
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                
                // Cost: 0 if we own it, 1 if empty
                int stepCost = (grid.getCell(neighbor) == player) ? 0 : 1;
                distances[neighbor] = currentDist + stepCost;
                
                queue.push(neighbor);
            }
        }
    }
    
    // Convert distance to connectivity score (higher = better)
    if (minDistanceToGoal == std::numeric_limits<int>::max()) {
        return -100.0; // Completely blocked
    }
    
    // Score: lower distance = higher connectivity
    return (HexGrid::BOARD_SIZE * 2.0) - minDistanceToGoal;
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
