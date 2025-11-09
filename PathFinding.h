#pragma once
#include "HexGrid.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>

class PathFinding {
public:
    static bool hasWinningPath(const HexGrid& grid, Player player);
    static double calculateConnectivity(const HexGrid& grid, Player player);
    static int countBridges(const HexGrid& grid, Player player);
};
