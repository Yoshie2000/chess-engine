#ifndef INCLUDE_EVALUATION_H
#define INCLUDE_EVALUATION_H

#include "types.h"
#include "board.h"
#include "search.h"

const Eval EVAL_MATE = 30000;
const Eval EVAL_MATE_IN_MAX_PLY = EVAL_MATE - MAX_PLY;

const Eval EVAL_INFINITE = 31000;

extern const Eval PIECE_VALUES[PIECE_TYPES];
extern const Eval PSQ[PIECE_TYPES][64];

constexpr Square psqIndex(Square square, Color side) {
    if (side == COLOR_BLACK)
        return 63 - ((63 - square) ^ (FILE_A & RANK_8));
    return 63 - square;
}

Eval evaluate(Board* board);

std::string formatEval(Eval value);

void debugEval(Board* board);

constexpr Eval mateIn(int ply) {
    return EVAL_MATE - ply;
}

constexpr Eval matedIn(int ply) {
    return -EVAL_MATE + ply;
}

#endif