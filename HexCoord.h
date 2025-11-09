#pragma once
#include <cmath>
#include <functional>

// Hexagonal coordinate in axial system
struct HexCoord {
    int q, r;
    
    HexCoord() : q(0), r(0) {}
    HexCoord(int q, int r) : q(q), r(r) {}
    
    bool operator==(const HexCoord& other) const {
        return q == other.q && r == other.r;
    }
    
    bool operator!=(const HexCoord& other) const {
        return !(*this == other);
    }
    
    int distance(const HexCoord& other) const {
        int dq = abs(q - other.q);
        int dr = abs(r - other.r);
        int ds = abs((q + r) - (other.q + other.r));
        return (dq + dr + ds) / 2;
    }
};

// Hash function for HexCoord
namespace std {
    template<> struct hash<HexCoord> {
        size_t operator()(const HexCoord& coord) const {
            return hash<int>()(coord.q) ^ (hash<int>()(coord.r) << 1);
        }
    };
}

enum class Player {
    NONE = 0,
    RED = 1,
    BLUE = 2
};

struct Move {
    HexCoord coord;
    Player player;
    
    Move() : coord(), player(Player::NONE) {}
    Move(HexCoord c, Player p) : coord(c), player(p) {}
};
