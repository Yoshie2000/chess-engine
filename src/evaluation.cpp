#include <iostream>

#include "types.h"
#include "board.h"
#include "evaluation.h"

Eval PIECE_VALUES[PIECE_TYPES] = {
    100,
    290,
    310,
    500,
    900,
    EVAL_MATE,
};

Eval PSQ[PIECE_TYPES][64] = {
    {
        // pawn
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    },
    {
        // knight
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    },
    {
        // bishop
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    },
    {
        // rook
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0
    },
    {
        // queen
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    },
    {
        // king
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
    }
};

template<Color side>
Eval evaluate(Board* board) {
    Eval result = 0;

    // Basic material evaluation
    for (Piece piece = 0; piece < PIECE_TYPES - 1; piece++) {
        result += PIECE_VALUES[piece] * board->stack->pieceCount[side][piece];
    }

    // PSQ
    for (Piece piece = 0; piece < PIECE_TYPES; piece++) {
        Bitboard pieceBB = board->byPiece[side][piece];
        while (pieceBB) {
            Square sq = popLSB(&pieceBB);
            if (side == COLOR_BLACK)
                sq = (63 - sq) ^ (FILE_A & RANK_8);
            result += PSQ[piece][sq] / 2;
        }
    }

    // Small bonus for lots of "vision"/"space"
    result += 10 * __builtin_popcountll(board->stack->attackedByColor[side]);

    return result;
}

Eval evaluate(Board* board) {
    if (board->stm == COLOR_WHITE)
        return evaluate<COLOR_WHITE>(board) - evaluate<COLOR_BLACK>(board);
    else
        return evaluate<COLOR_BLACK>(board) - evaluate<COLOR_WHITE>(board);
}

void debugEval(Board* board) {
    if (board->stm == COLOR_WHITE)
        std::cout << evaluate(board) << " = " << evaluate<COLOR_WHITE>(board) << " - " << evaluate<COLOR_BLACK>(board) << std::endl;
    else
        std::cout << evaluate(board) << " = " << evaluate<COLOR_BLACK>(board) << " - " << evaluate<COLOR_WHITE>(board) << std::endl;
}

std::string formatEval(Eval value) {
    std::string evalString;
    if (value >= EVAL_MATE_IN_MAX_PLY) {
        evalString = "mate " + std::to_string(EVAL_MATE - value);
    }
    else if (value <= -EVAL_MATE_IN_MAX_PLY) {
        evalString = "mate " + std::to_string(-EVAL_MATE - value);
    }
    else {
        evalString = "cp " + std::to_string(value);
    }
    return evalString;
}