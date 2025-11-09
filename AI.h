#pragma once
#include "HexGrid.h"
#include "Minimax.h"
#include "MonteCarlo.h"

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

struct MoveInfo {
    Move move;
    double score;
    int nodesEvaluated;
    int simulations;
    double winRate;
    int thinkTime;
};

class AI {
public:
    AI(Difficulty difficulty = Difficulty::MEDIUM);
    
    void setDifficulty(Difficulty difficulty);
    MoveInfo calculateMove(HexGrid& grid);
    
private:
    Difficulty difficulty;
    Minimax minimax;
    MonteCarlo monteCarlo;
};
