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
    uint16_t hash = 0;
    Move bestMove = MOVE_NONE;
    uint8_t depth = 0;
    uint8_t flags = TT_NOBOUND;
    Eval eval = EVAL_NONE;
    Eval value = EVAL_NONE;

    void update(uint64_t _hash, Move _bestMove, uint8_t _depth, Eval _eval, Eval _value, bool wasPv, int flags) {
        if (_depth >= depth || bestMove == MOVE_NONE) {
            hash = (uint16_t)_hash;
            bestMove = _bestMove;
            depth = _depth;
            value = _value;
            eval = _eval;
            flags = (uint8_t)(flags + (wasPv << 2));
        }
    }
};

#define CLUSTER_SIZE 3

struct TTCluster {
    TTEntry entry[CLUSTER_SIZE];
    char padding[2];
};

class TranspositionTable {

    TTCluster* table;
    size_t clusterCount;

public:

    TranspositionTable() {
        size_t mb = 64;

        clusterCount = mb * 1024 * 1024 / sizeof(TTCluster);
        table = static_cast<TTCluster*>(std::aligned_alloc(sizeof(TTCluster), clusterCount * sizeof(TTCluster)));

        clear();
    }

    ~TranspositionTable() {
        std::free(table);
    }

    constexpr size_t index(uint64_t hash) {
        __extension__ using uint128 = unsigned __int128;
        return ((uint128)hash * (uint128)clusterCount) >> 64;
    }

    TTEntry* probe(uint64_t hash, bool* found) {
        // Find cluster
        TTCluster* cluster = &table[index(hash)];

        int smallestDepth = 0;
        for (int i = 0; i < CLUSTER_SIZE; i++) {
            if (cluster->entry[i].hash == (uint16_t)hash) {
                *found = true;
                return &cluster->entry[i];
            }
            if (cluster->entry[i].depth < cluster->entry[smallestDepth].depth)
                smallestDepth = i;
        }
        *found = false;
        return &cluster->entry[smallestDepth];
    }

    void clear() {
        for (size_t i = 0; i < clusterCount; i++) {
            table[i] = TTCluster();
        }
    }

    void prefetch(uint64_t hash) {
        __builtin_prefetch((char*) &table[index(hash)]);
    }

};

extern TranspositionTable TT;

void initHistory();
extern int quietHistory[2][64][64];

#endif