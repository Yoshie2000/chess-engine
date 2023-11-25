#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <string>

#include "move.h"
#include "types.h"
#include "board.h"
#include <iostream>
#include <cassert>

const Bitboard FILE_A = 0x0101010101010101;
const Bitboard FILE_B = 0x0202020202020202;
const Bitboard FILE_C = 0x0404040404040404;
const Bitboard FILE_D = 0x0808080808080808;
const Bitboard FILE_E = 0x1010101010101010;
const Bitboard FILE_F = 0x2020202020202020;
const Bitboard FILE_G = 0x4040404040404040;
const Bitboard FILE_H = 0x8080808080808080;

const Bitboard RANK_1 = 0x00000000000000FF;
const Bitboard RANK_2 = 0x000000000000FF00;
const Bitboard RANK_3 = 0x0000000000FF0000;
const Bitboard RANK_4 = 0x00000000FF000000;
const Bitboard RANK_5 = 0x000000FF00000000;
const Bitboard RANK_6 = 0x0000FF0000000000;
const Bitboard RANK_7 = 0x00FF000000000000;
const Bitboard RANK_8 = 0xFF00000000000000;

void startpos(Board* result) {
    parseFen(result, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

size_t parseFen(Board* board, std::string fen) {
    Square currentSquare = 56;
    uint8_t currentRank = 7;
    size_t i = 0;

    // Reset everything (there might be garbage data)
    board->board = 0;
    for (Square s = 0; s < 64; s++) {
        board->pieces[s] = NO_PIECE;
    }
    for (Color c = 0; c <= 1; c++) {
        board->byColor[c] = C64(0);
        for (Piece p = 0; p < PIECE_TYPES; p++) {
            board->byPiece[c][p] = C64(0);
        }
    }
    board->stack->capturedPiece = NO_PIECE;

    // Board position and everything
    Bitboard currentSquareBB = C64(1) << currentSquare;
    for (i = 0; i < fen.length(); i++) {
        char c = fen.at(i);
        if (c == ' ') {
            i++;
            break;
        }

        if (currentSquare < 64)
            currentSquareBB = C64(1) << currentSquare;
        switch (c) {
        case 'p':
            board->board |= currentSquareBB;
            board->byColor[COLOR_BLACK] |= currentSquareBB;
            board->byPiece[COLOR_BLACK][PIECE_PAWN] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_PAWN;
            currentSquare++;
            break;
        case 'P':
            board->board |= currentSquareBB;
            board->byColor[COLOR_WHITE] |= currentSquareBB;
            board->byPiece[COLOR_WHITE][PIECE_PAWN] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_PAWN;
            currentSquare++;
            break;
        case 'n':
            board->board |= currentSquareBB;
            board->byColor[COLOR_BLACK] |= currentSquareBB;
            board->byPiece[COLOR_BLACK][PIECE_KNIGHT] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_KNIGHT;
            currentSquare++;
            break;
        case 'N':
            board->board |= currentSquareBB;
            board->byColor[COLOR_WHITE] |= currentSquareBB;
            board->byPiece[COLOR_WHITE][PIECE_KNIGHT] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_KNIGHT;
            currentSquare++;
            break;
        case 'b':
            board->board |= currentSquareBB;
            board->byColor[COLOR_BLACK] |= currentSquareBB;
            board->byPiece[COLOR_BLACK][PIECE_BISHOP] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_BISHOP;
            currentSquare++;
            break;
        case 'B':
            board->board |= currentSquareBB;
            board->byColor[COLOR_WHITE] |= currentSquareBB;
            board->byPiece[COLOR_WHITE][PIECE_BISHOP] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_BISHOP;
            currentSquare++;
            break;
        case 'r':
            board->board |= currentSquareBB;
            board->byColor[COLOR_BLACK] |= currentSquareBB;
            board->byPiece[COLOR_BLACK][PIECE_ROOK] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_ROOK;
            currentSquare++;
            break;
        case 'R':
            board->board |= currentSquareBB;
            board->byColor[COLOR_WHITE] |= currentSquareBB;
            board->byPiece[COLOR_WHITE][PIECE_ROOK] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_ROOK;
            currentSquare++;
            break;
        case 'q':
            board->board |= currentSquareBB;
            board->byColor[COLOR_BLACK] |= currentSquareBB;
            board->byPiece[COLOR_BLACK][PIECE_QUEEN] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_QUEEN;
            currentSquare++;
            break;
        case 'Q':
            board->board |= currentSquareBB;
            board->byColor[COLOR_WHITE] |= currentSquareBB;
            board->byPiece[COLOR_WHITE][PIECE_QUEEN] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_QUEEN;
            currentSquare++;
            break;
        case 'k':
            board->board |= currentSquareBB;
            board->byColor[COLOR_BLACK] |= currentSquareBB;
            board->byPiece[COLOR_BLACK][PIECE_KING] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_KING;
            currentSquare++;
            break;
        case 'K':
            board->board |= currentSquareBB;
            board->byColor[COLOR_WHITE] |= currentSquareBB;
            board->byPiece[COLOR_WHITE][PIECE_KING] |= currentSquareBB;
            board->pieces[currentSquare] = PIECE_KING;
            currentSquare++;
            break;
        case '/':
            currentRank--;
            currentSquare = 8 * currentRank;
            break;
        default: // Number
            currentSquare += (int)(c)-48;
            break;
        }
    }

    // Side to move
    board->stm = fen.at(i) == 'w' ? COLOR_WHITE : COLOR_BLACK;
    i += 2;

    // Castling
    board->stack->castling = 0;
    for (; i < fen.length(); i++) {
        char c = fen.at(i);

        if (c == '-') {
            i += 2;
            break;
        }
        else if (c == ' ') {
            i++;
            break;
        }

        switch (c) {
        case 'k':
            board->stack->castling |= 0x4;
            break;
        case 'K':
            board->stack->castling |= 0x1;
            break;
        case 'q':
            board->stack->castling |= 0x8;
            break;
        case 'Q':
            board->stack->castling |= 0x4;
            break;
        case ' ':
        default:
            printf("Weird char in fen castling: %c (index %ld)\n", c, i);
            exit(-1);
            break;
        }
    }

    // en passent
    if (fen.at(i) == '-') {
        board->stack->enpassantTarget = C64(0);
        i += 2;
    }
    else {
        char epTargetString[2] = { fen.at(i), fen.at(i + 1) };
        Square epTargetSquare = stringToSquare(epTargetString);
        board->stack->enpassantTarget = C64(1) << epTargetSquare;
        i += 3;
    }

    // 50 move rule
    std::string rule50String = "--";
    int rule50tmp = 0;
    while (fen.at(i) != ' ') {
        rule50String.replace(rule50tmp++, 1, 1, fen.at(i++));
    }
    if (rule50String.at(1) == '-') {
        board->rule50_ply = (int)(rule50String.at(0)) - 48;
    }
    else {
        board->rule50_ply = 10 * ((int)(rule50String.at(0)) - 48) + ((int)(rule50String.at(1)) - 48);
    }
    i++;

    // Move number
    std::string plyString = "---";
    int plyTmp = 0;
    while (i < fen.length() && fen.at(i) != ' ') {
        plyString.replace(plyTmp++, 1, 1, fen.at(i++));
    }
    if (plyString.at(1) == '-') {
        board->ply = (int)(plyString.at(0)) - 48;
    }
    else if (plyString.at(2) == '-') {
        board->ply = 10 * ((int)(plyString.at(0)) - 48) + ((int)(plyString.at(1)) - 48);
    }
    else {
        board->ply = 100 * ((int)(plyString.at(0)) - 48) + 10 * ((int)(plyString.at(1)) - 48) + ((int)(plyString.at(2)) - 48);
    }

    // Compute attackers
    for (Color side = 0; side <= 1; side++) {
        Bitboard attackers = C64(0);
        for (Piece piece = 0; piece < PIECE_TYPES; piece++) {
            board->stack->attackedByPiece[side][piece] = attackedSquaresByPiece(board, side, piece);
            attackers |= board->stack->attackedByPiece[side][piece];
        }
        board->stack->attackedByColor[side] = attackers;
    }

    return i;
}

void castlingRookSquares(Board* board, Square origin, Square target, Square* rookOrigin, Square* rookTarget) {
    if (board->stm == COLOR_WHITE) {
        if (target > origin) { // Kingside White
            *rookOrigin = 7;
            *rookTarget = 5;
        }
        else { // Queenside White
            *rookOrigin = 0;
            *rookTarget = 3;
        }
        board->stack->castling &= 0xC; // Clear flags for white
    }
    else {
        if (target > origin) { // Kingside Black
            *rookOrigin = 63;
            *rookTarget = 61;
        }
        else { // Queenside Black
            *rookOrigin = 56;
            *rookTarget = 59;
        }
        board->stack->castling &= 0x3; // Clear flags for black
    }
}

void doMove(Board* board, BoardStack* newStack, Move move) {
    newStack->previous = board->stack;
    board->stack = newStack;
    memcpy(board->stack->attackedByPiece, board->stack->previous->attackedByPiece, sizeof(Bitboard) * 14 + sizeof(uint8_t));

    Square origin = moveOrigin(move);
    Square target = moveTarget(move);

    assert(origin < 64 && target < 64);

    newStack->capturedPiece = board->pieces[target];
    Square captureTarget = target;
    Bitboard captureTargetBB = C64(1) << captureTarget;

    Piece piece = board->pieces[origin];
    Piece promotionPiece = NO_PIECE;

    Bitboard originBB = C64(1) << origin;
    Bitboard targetBB = C64(1) << target;
    Bitboard fromTo = originBB | targetBB;

    board->board ^= fromTo;
    board->byColor[board->stm] ^= fromTo;
    board->byPiece[board->stm][piece] ^= fromTo;

    board->pieces[origin] = NO_PIECE;
    board->pieces[target] = piece;

    // This move is en passent
    Move specialMove = move & 0x3000;
    if (__builtin_expect(specialMove == MOVE_ENPASSANT, 0)) {
        newStack->capturedPiece = PIECE_PAWN;
        captureTarget = target - UP[board->stm];

        assert(captureTarget < 64);

        captureTargetBB = C64(1) << captureTarget;
        board->pieces[captureTarget] = NO_PIECE; // remove the captured pawn
        board->board ^= captureTargetBB;
    }

    // Handle capture
    if (newStack->capturedPiece != NO_PIECE) {
        board->board |= targetBB; // Put piece on target square (in case xor didn't work)
        board->byColor[1 - board->stm] ^= captureTargetBB; // take away the captured piece
        board->byPiece[1 - board->stm][newStack->capturedPiece] ^= captureTargetBB;
    }

    // En passent square
    newStack->enpassantTarget = 0;
    if (__builtin_expect(piece == PIECE_PAWN && (origin ^ target) == 16, 0)) {
        assert(target - UP[board->stm] < 64);
        newStack->enpassantTarget = C64(1) << (target - UP[board->stm]);
    }

    // This move is castling
    if (__builtin_expect(specialMove == MOVE_CASTLING, 0)) {
        Square rookOrigin, rookTarget;
        castlingRookSquares(board, origin, target, &rookOrigin, &rookTarget);
        assert(rookOrigin < 64 && rookTarget < 64);

        Bitboard rookFromToBB = (C64(1) << rookOrigin) | (C64(1) << rookTarget);
        board->pieces[rookOrigin] = NO_PIECE;
        board->pieces[rookTarget] = PIECE_ROOK;
        board->board ^= rookFromToBB;
        board->byColor[board->stm] ^= rookFromToBB;
        board->byPiece[board->stm][PIECE_ROOK] ^= rookFromToBB;
    }

    // This move is promotion
    if (__builtin_expect(specialMove == MOVE_PROMOTION, 0)) {
        promotionPiece = PROMOTION_PIECE[move >> 14];
        board->byPiece[board->stm][piece] ^= targetBB;
        board->byPiece[board->stm][promotionPiece] ^= targetBB;

        board->pieces[target] = promotionPiece;
    }

    // Unset castling flags if necessary
    if (piece == PIECE_KING) {
        if (board->stm == COLOR_WHITE)
            board->stack->castling &= 0xC;
        else
            board->stack->castling &= 0x3;
    }
    else if (piece == PIECE_ROOK || newStack->capturedPiece == PIECE_ROOK) {
        switch (piece == PIECE_ROOK ? origin : captureTarget) {
        case 0:
            board->stack->castling &= ~0x2; // Queenside castle white
            break;
        case 7:
            board->stack->castling &= ~0x1; // Kingside castle white
            break;
        case 56:
            board->stack->castling &= ~0x8; // Queenside castle black
            break;
        case 63:
            board->stack->castling &= ~0x4; // Kingside castle black
            break;
        default:
            break;
        }
    }

    Color side;
    Bitboard attackers;
    Piece _piece;
    for (side = 0; side <= 1; side++) {
        attackers = C64(0);
        for (_piece = 0; _piece < PIECE_TYPES; _piece++) {
            // If from or to are attacked, or this piece type was moved or captured, regenerate
            if (
                (board->stack->attackedByPiece[side][_piece] & (fromTo | captureTargetBB)) ||
                (side == board->stm && piece == _piece) ||
                (side != board->stm && _piece == newStack->capturedPiece) ||
                (_piece == promotionPiece)
                ) {
                board->stack->attackedByPiece[side][_piece] = attackedSquaresByPiece(board, side, _piece);
            }
            attackers |= board->stack->attackedByPiece[side][_piece];
        }
        board->stack->attackedByColor[side] = attackers;
    }

    board->stm = 1 - board->stm;
}

void undoMove(Board* board, Move move) {
    board->stm = 1 - board->stm;

    Square origin = moveOrigin(move);
    Square target = moveTarget(move);
    Square captureTarget = target;

    assert(origin < 64 && target < 64);

    Bitboard originBB = C64(1) << origin;
    Bitboard targetBB = C64(1) << target;
    Bitboard fromTo = originBB | targetBB;
    Bitboard captureTargetBB = C64(1) << captureTarget;

    board->board ^= fromTo;
    board->byColor[board->stm] ^= fromTo;

    Piece piece = board->pieces[target];

    board->byPiece[board->stm][piece] ^= fromTo;

    board->pieces[target] = NO_PIECE;
    board->pieces[origin] = piece;

    // This move is en passent
    Move specialMove = move & 0x3000;
    if (specialMove == MOVE_ENPASSANT) {
        captureTarget = target - UP[board->stm];

        assert(captureTarget < 64);

        captureTargetBB = C64(1) << captureTarget;
        board->board ^= captureTargetBB;
    }

    // Handle capture
    if (board->stack->capturedPiece != NO_PIECE) {
        board->pieces[captureTarget] = board->stack->capturedPiece;
        board->board |= captureTargetBB;
        board->byColor[1 - board->stm] ^= captureTargetBB;
        board->byPiece[1 - board->stm][board->stack->capturedPiece] ^= captureTargetBB;
    }

    // Castling
    if (__builtin_expect(specialMove == MOVE_CASTLING, 0)) {
        Square rookOrigin, rookTarget;
        castlingRookSquares(board, origin, target, &rookOrigin, &rookTarget);

        assert(rookOrigin < 64 && rookTarget < 64);

        Bitboard rookFromToBB = (C64(1) << rookOrigin) | (C64(1) << rookTarget);
        board->pieces[rookOrigin] = PIECE_ROOK;
        board->pieces[rookTarget] = NO_PIECE;
        board->board ^= rookFromToBB;
        board->byColor[board->stm] ^= rookFromToBB;
        board->byPiece[board->stm][PIECE_ROOK] ^= rookFromToBB;
    }

    // This move is promotion
    if (__builtin_expect(specialMove == MOVE_PROMOTION, 0)) {
        board->byPiece[board->stm][piece] ^= originBB;
        board->byPiece[board->stm][PIECE_PAWN] ^= originBB;

        board->pieces[origin] = PIECE_PAWN;
    }

    board->stack = board->stack->previous;
}

Bitboard pawnAttacksLeft(Board* board, Color side) {
    Bitboard pawns = board->byPiece[side][PIECE_PAWN];
    return side == COLOR_WHITE ?
        ((pawns & (~FILE_A)) << 7) :
        ((pawns & (~FILE_A)) >> 9);
}

Bitboard pawnAttacksRight(Board* board, Color side) {
    Bitboard pawns = board->byPiece[side][PIECE_PAWN];
    return side == COLOR_WHITE ?
        ((pawns & (~FILE_H)) << 9) :
        ((pawns & (~FILE_H)) >> 7);
}

Bitboard pawnAttacks(Board* board, Color side) {
    return pawnAttacksLeft(board, side) | pawnAttacksRight(board, side);
}

Bitboard knightAttacks(Bitboard knightBB) {
    Bitboard l1 = (knightBB >> 1) & C64(0x7f7f7f7f7f7f7f7f);
    Bitboard l2 = (knightBB >> 2) & C64(0x3f3f3f3f3f3f3f3f);
    Bitboard r1 = (knightBB << 1) & C64(0xfefefefefefefefe);
    Bitboard r2 = (knightBB << 2) & C64(0xfcfcfcfcfcfcfcfc);
    Bitboard h1 = l1 | r1;
    Bitboard h2 = l2 | r2;
    return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
}

Bitboard knightAttacksAll(Board* board, Color side) {
    Bitboard knights = board->byPiece[side][PIECE_KNIGHT];
    return knightAttacks(knights);
}

Bitboard kingAttacks(Board* board, Color color) {
    Bitboard attacksBB = C64(0);
    Square origin = lsb(board->byPiece[color][PIECE_KING]);

    int8_t direction;
    Square lastSquare, toSquare;
    Bitboard toSquareBB;

    for (direction = DIRECTIONS[PIECE_KING][0]; direction <= DIRECTIONS[PIECE_KING][1]; direction++) {
        lastSquare = LASTSQ_TABLE[origin][direction];
        toSquare = origin + DIRECTION_DELTAS[direction];
        if (toSquare >= 64) continue;

        toSquareBB = C64(1) << toSquare;
        if (origin != lastSquare && toSquareBB)
            attacksBB |= toSquareBB;
    }
    return attacksBB;
}

Bitboard slidingPieceAttacks(Board* board, Bitboard pieceBB) {
    assert(pieceBB > 0);

    Bitboard attacksBB = C64(0);
    Square origin = lsb(pieceBB);
    Piece pieceType = board->pieces[origin];

    int8_t delta, direction;
    Square lastSquare, toSquare;
    Bitboard toSquareBB;

    assert(pieceType < NO_PIECE);
    
    for (direction = DIRECTIONS[pieceType][0]; direction <= DIRECTIONS[pieceType][1]; direction++) {
        delta = DIRECTION_DELTAS[direction];
        lastSquare = LASTSQ_TABLE[origin][direction];

        if (origin != lastSquare)
            for (toSquare = origin + delta; toSquare < 64; toSquare += delta) {
                toSquareBB = C64(1) << toSquare;
                attacksBB |= toSquareBB;
                if ((board->board & toSquareBB) || (toSquare == lastSquare))
                    break;
            }
    }
    return attacksBB;
}

Bitboard slidingPieceAttacksAll(Board* board, Color side, Piece pieceType) {
    Bitboard attacksBB = C64(0);

    Bitboard pieces = board->byPiece[side][pieceType];
    while (pieces) {
        Bitboard pieceBB = C64(1) << popLSB(&pieces);
        attacksBB |= slidingPieceAttacks(board, pieceBB);
    }
    return attacksBB;
}

bool isSquareAttacked(Board* board, Square square, Color side) {
    Bitboard squareBB = C64(1) << square;

    if (pawnAttacks(board, side) & squareBB)
        return true;
    if (knightAttacksAll(board, side) & squareBB)
        return true;
    if (kingAttacks(board, side) & squareBB)
        return true;
    if (slidingPieceAttacksAll(board, side, PIECE_BISHOP) & squareBB)
        return true;
    if (slidingPieceAttacksAll(board, side, PIECE_ROOK) & squareBB)
        return true;
    if (slidingPieceAttacksAll(board, side, PIECE_QUEEN) & squareBB)
        return true;
    return false;
}

Bitboard attackedSquares(Board* board, Color side) {
    return
        pawnAttacks(board, side) |
        knightAttacksAll(board, side) |
        kingAttacks(board, side) |
        slidingPieceAttacksAll(board, side, PIECE_BISHOP) |
        slidingPieceAttacksAll(board, side, PIECE_ROOK) |
        slidingPieceAttacksAll(board, side, PIECE_QUEEN);
}

Bitboard attackedSquaresByPiece(Board* board, Color side, Piece pieceType) {
    switch (pieceType) {
    case PIECE_PAWN:
        return pawnAttacks(board, side);
    case PIECE_KNIGHT:
        return knightAttacksAll(board, side);
    case PIECE_KING:
        return kingAttacks(board, side);
    case PIECE_BISHOP:
        return slidingPieceAttacksAll(board, side, PIECE_BISHOP);
    case PIECE_ROOK:
        return slidingPieceAttacksAll(board, side, PIECE_ROOK);
    default:
        return slidingPieceAttacksAll(board, side, PIECE_QUEEN);
    }
}

bool isInCheck(Board* board, Color side) {
    Square kingSquare = lsb(board->byPiece[side][PIECE_KING]);
    Bitboard attackedEnemy = board->stack->attackedByColor[1 - side];
    return (C64(1) << kingSquare) & attackedEnemy;
}

void debugBoard(Board* board) {
    for (int rank = 7; rank >= 0; rank--) {

        printf("-");
        for (int file = 0; file <= 15; file++) {
            printf(" -");
        }
        printf("\n");

        for (int file = 0; file <= 7; file++) {

            // Get piece at index
            int idx = file + 8 * rank;
            // printf("| ");
            // if (board->pieces[idx] == 6)
            //     printf(" ");
            // else
            //     printf("%d", board->pieces[idx]);
            // printf(" ");
            Bitboard mask = C64(1) << idx;
            if ((board->stack->enpassantTarget & mask) != 0)
                printf("| E ");
            else if ((board->board & mask) == 0)
                printf("|   ");
            else if ((board->byPiece[COLOR_WHITE][PIECE_PAWN] & mask) != 0)
                printf("| ♙ ");
            else if ((board->byPiece[COLOR_BLACK][PIECE_PAWN] & mask) != 0)
                printf("| ♟︎ ");
            else if ((board->byPiece[COLOR_WHITE][PIECE_KNIGHT] & mask) != 0)
                printf("| ♘ ");
            else if ((board->byPiece[COLOR_BLACK][PIECE_KNIGHT] & mask) != 0)
                printf("| ♞ ");
            else if ((board->byPiece[COLOR_WHITE][PIECE_BISHOP] & mask) != 0)
                printf("| ♗ ");
            else if ((board->byPiece[COLOR_BLACK][PIECE_BISHOP] & mask) != 0)
                printf("| ♝ ");
            else if ((board->byPiece[COLOR_WHITE][PIECE_ROOK] & mask) != 0)
                printf("| ♖ ");
            else if ((board->byPiece[COLOR_BLACK][PIECE_ROOK] & mask) != 0)
                printf("| ♜ ");
            else if ((board->byPiece[COLOR_WHITE][PIECE_QUEEN] & mask) != 0)
                printf("| ♕ ");
            else if ((board->byPiece[COLOR_BLACK][PIECE_QUEEN] & mask) != 0)
                printf("| ♛ ");
            else if ((board->byPiece[COLOR_WHITE][PIECE_KING] & mask) != 0)
                printf("| ♔ ");
            else if ((board->byPiece[COLOR_BLACK][PIECE_KING] & mask) != 0)
                printf("| ♚ ");
            else
                printf("| ? ");
        }
        printf("|\n");
    }
    printf("-");
    for (int file = 0; file <= 15; file++) {
        printf(" -");
    }
    printf("\n");
}

void debugBitboard(Bitboard bb) {
    for (int rank = 7; rank >= 0; rank--) {

        printf("-");
        for (int file = 0; file <= 15; file++) {
            printf(" -");
        }
        printf("\n");

        for (int file = 0; file <= 7; file++) {

            // Get piece at index
            int idx = file + 8 * rank;
            Bitboard mask = 1L << idx;
            if ((bb & mask) == 0)
                printf("|   ");
            else
                printf("| X ");
        }
        printf("|\n");
    }
    printf("-");
    for (int file = 0; file <= 15; file++) {
        printf(" -");
    }
    printf("\n");
}