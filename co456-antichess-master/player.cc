#include "player.h"
#include  <random>
#include  <vector>
#include <cfloat>
#include <iostream>

#include <iostream>
using namespace std;

double Player::evaluateBoard(ChessBoard& board) {
    double black = 0;
    double white = 0;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            ChessPiece* piece = board.getPiece(vec2(x, y));
            if (piece == nullptr || piece->type == EMPTY) {
                continue;
            }
            double* score = (piece->white)? &white : &black;
            switch(piece->type) {
                case KING:
                    *score += 200;
                    break;
                case QUEEN:
                    *score += 9;
                    break;
                case CASTLE:
                    *score += 5;
                    break;
                case BISHOP:
                    *score += 3;
                    break;
                case KNIGHT:
                    *score += 3;
                    break;
                case PAWN:
                    *score += 1;
                    break;
                default:
                    break;
            }
        }
    }

    // mobility
    bool color = board.isWhite();
    if (num_moves > 5 && abs(white - black) < 2) {
        board.setWhite(true);
        white += 0.1 * board.genMoves().size();
        board.setWhite(false);
        black += 0.1 * board.genMoves().size();
        board.setWhite(color);
    }

    if (color) {
        return white - black;
    }
    return black - white;
}

double Player::negmaxAlphaBeta(ChessBoard& board, int depth, double alpha, double beta, bool quiescent) {
    if ((depth <= 0 && !quiescent) || depth < -2) {
        return evaluateBoard(board);
    }

    vector<Move> moves = board.genMoves();

    double best = -DBL_MAX, temp;
    bool white = board.isWhite();

    for (Move& m : moves) {
        board.move(m);

        if (!board.inCheck(white)) {
            quiescent = (m.moveType & CAPTURE);
            temp = -negmaxAlphaBeta(board, depth - 1, -beta, -alpha, quiescent);

            if (temp > best) {
                best = temp;
                if (temp > alpha) {
                    alpha = temp;
                }
            }
        }

        board.unmove();


        if (alpha >= beta) {
            break;
        }
    }

    return best;
}

Player::Player(bool white, ChessBoard* board) : white(white), board(board), num_moves(0) {
    surrender = false;
}

Move Player::makeMove() {
    vector<Move> moves = board->genMoves();
    vector<Move> bestMoves;

    double best = -DBL_MAX, temp;
    double alpha = -DBL_MAX;
    for (Move& m : moves) {
        temp = -DBL_MAX;
        board->move(m);

        if (!board->inCheck(this->white)) {
            bool quiescent = (m.moveType & CAPTURE);
            temp = -negmaxAlphaBeta(*board, SEARCH_DEPTH, -DBL_MAX, -alpha, quiescent);

            if (temp > best) {
                best = temp;
                bestMoves.clear();
            }

            if (temp > alpha) {
                alpha = temp;
            }

            if (temp == best) {
                bestMoves.push_back(m);
            }
        }
        board->unmove();
    }

    if (surrender || bestMoves.size() == 0) {
        surrender = true;
        if (this->white) {
            cout << "0-1" << endl;
        } else {
            cout << "1-0" << endl;
        }
        return Move(vec2(0,0), vec2(0,0));
    }

    // cout << "[" << white << "] Score : " << best << endl;
    Move move = bestMoves[rand() % bestMoves.size()];

    num_moves++;
    board->move(move);

    return move;
}
