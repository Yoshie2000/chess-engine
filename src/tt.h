#ifndef TT_H_INCLUDED
#define TT_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
#include <string.h>

#include "types.h"
#include "move.h"
#include "evaluation.h"

#define TT_NOBOUND 0
#define TT_UPPERBOUND 1
#define TT_LOWERBOUND 2
#define TT_EXACTBOUND 3

extern uint64_t ZOBRIST_PIECE_SQUARES[PIECE_TYPES][64];
extern uint64_t ZOBRIST_STM_BLACK;
extern uint64_t ZOBRIST_CASTLING[16]; // 2^4
extern uint64_t ZOBRIST_ENPASSENT[8]; // 8 files

void initZobrist();

struct TTEntry {
    uint16_t hash;
    Move bestMove;
    uint8_t depth;
    uint8_t flags;
    Eval eval;
    Eval value;

    TTEntry() {
        hash = 0;
        bestMove = MOVE_NONE;
        depth = 0;
        flags = TT_NOBOUND;
        eval = EVAL_NONE;
        value = EVAL_NONE;
    }

    void update(uint64_t _hash, Move _bestMove, uint8_t _depth, Eval _eval, Eval _value, bool wasPv, int flags) {
        hash = (uint16_t)_hash;
        bestMove = _bestMove;
        depth = _depth;
        value = _value;
        eval = _eval;
        flags = (uint8_t) (flags + (wasPv << 2));
    }
};

class TranspositionTable {

    std::vector<TTEntry> table;
    size_t entryCount;

public:

    TranspositionTable() {
        size_t mb = 64;

        entryCount = mb * 1024 * 1024 / sizeof(TTEntry);
        table.resize(entryCount);

        clear();
    }

    TTEntry* probe(uint64_t hash, bool* found) {
        uint64_t index = ttIndex(hash);
        *found = (uint16_t)hash == table[index].hash;
        return &table[index];
    }

    uint64_t ttIndex(uint64_t hash) {
#ifdef __SIZEOF_INT128__
        return static_cast<uint64_t>(((static_cast<__uint128_t>(hash) * static_cast<__uint128_t>(table.size())) >> 64));
#else 
        return hash % table.size();
#endif
    }

    void clear() {
        table.clear();
        table.resize(entryCount);
    }

};

extern TranspositionTable TT;

void initHistory();
extern int quietHistory[2][64][64];

#endif