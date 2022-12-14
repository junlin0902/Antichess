#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "board.h"
#define SEARCH_DEPTH 6

class Player {
public:
    Player(bool white, ChessBoard* board);
    Move makeMove();
    double evaluateBoard(ChessBoard& board);
    double negmaxAlphaBeta(ChessBoard& board, int depth, double alpha, double beta, bool quiescent);

    bool surrender;
private:
    bool white;
    ChessBoard* board;
    int num_moves;
};

#endif
