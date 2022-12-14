#include "board.h"
#include <iostream>
#include <cmath>
#include <algorithm>

#include <assert.h>
using namespace std;

static vec2 INVALID_POS(-1, -1);

vec2::vec2() : x(0), y(0) {}
vec2::vec2(short x, short y) : x(x), y(y) {}

void vec2::set(short x, short y) {
    this->x = x;
    this->y = y;
}

vec2 vec2::add(const vec2& v) const {
    vec2 ret;

    ret.x = v.x + this->x;
    ret.y = v.y + this->y;

    return ret;
}

void vec2::neg() {
    x = -x;
    y = -y;
}

bool vec2::within() const {
    if (x < 8 && y < 8 && x >= 0 && y >= 0) {
        return true;
    }
    return false;
}

UnMove::UnMove(vec2 start, vec2 end, short moveType, ChessPieceType removedPiece)
: start(start), end(end), moveType(moveType), removed(removedPiece)
{}

Move::Move(vec2 start, vec2 end) : start(start), end(end), score(0) {
    moveType = 0;
    promotion = EMPTY;
}

Move::Move(vec2 start, vec2 end, short moveType) : start(start), end(end), moveType(moveType), score(0) {
    if (moveType == 4) {
        promotion = QUEEN;
    }
}

bool operator<(Move const & a, Move const & b) {
    return a.score > b.score;
}

string Move::toString() {
    string ret = "";

    ret += start.x + 'a';
    ret += start.y + '1';
    ret += end.x + 'a';
    ret += end.y + '1';

    if (moveType & PROMOTION) {
        ret += 'Q';
    }

    return ret;
}

void ChessBoard::genPieceDirectedMove(std::vector<Move>& moves, vec2 from, vec2 d, bool multi) {
    ChessPieceType type = board[from.x][from.y]->type;
    vec2 dir = d;
    do {
        vec2 to = from.add(dir);

        if (!to.within()) { // Out of bounds
            break;
        }

        short moveType = 0;

        ChessPiece* cp = board[to.x][to.y];
        if (cp != 0) {
            if (cp->white == this->white) { // Can't move to friendly
                break;
            }
            moveType = CAPTURE; // Capture
        } else {
            moveType = QUIET;
        }

        if (type == PAWN) {
            if (cp != 0 && dir.x == 0) { // Cannot move straight if there is an piece
                break;
            }

            if (cp == 0 && dir.x != 0) // Cannot move diagonal if there is no piece
                break;

            if (multi && abs(dir.y) >= 2) { // Second pawn move
                // Invalid for second pawn move
                if ((white && from.y > 1) || (!white && from.y < 6) || abs(dir.y) > 2)
                    break;
            }

            // Pawn Promotion
            if ((white && to.y == 7) || (!white && to.y == 0)) {
                moveType |= PROMOTION;
            }
        }

        Move m(from, to, moveType);
        if ((moveType & PROMOTION) != 0) {
            m.promotion = QUEEN;
        }

        moves.push_back(m);

        if (cp != 0) { // Caputre Move
            break;
        }

        dir = dir.add(d);
    } while (multi);
}

void ChessBoard::genPieceMove(std::vector<Move>& moves, vec2 p) {
    vec2 d(0, 0);
    ChessPieceType type = board[p.x][p.y]->type;

    switch(type) {
        case KING: {
             for (int sign = 0; sign < 2; sign ++) {
                d.set(1, 0); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, false);
                d.set(0, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, false);
                d.set(1, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, false);
                d.set(-1, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, false);
            }
            break;
        }
        case QUEEN:
            for (int sign = 0; sign < 2; sign ++) {
                d.set(1, 0); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, true);
                d.set(0, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, true);
                d.set(1, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, true);
                d.set(-1, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, true);
            }
            break;
        case CASTLE: {
            for (int sign = 0; sign < 2; sign ++) {
                d.set(1, 0); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, true);
                d.set(0, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, true);
            }

            if ((white && p.y != 0) || (!white && p.y != 7)) {
                break;
            }
            if (p.x != 0 || p.x != 7) {
                break;
            }

            int side = white ? 0 : 1;
            int num = p.x == 0 ? 0 : 1;
            if (!meta.inCheck[side] && meta.castling[side][num]) {
                // check empty
                int x = num == 0 ? 1 : 6;
                int y = white ? 0 : 7;
                int incre = x == 1 ? 1 : -1;
                bool canCastling = true;
                while (x != 4) {
                    if (board[x][y] != 0) {
                        canCastling = false;
                        break;
                    }
                    x += incre;
                }

                if (canCastling) {
                    moves.push_back(Move(vec2(4, p.y), vec2(num == 0 ? 2 : 6 ,p.y), CASTLING));
                }
            }
            break;
        }
        case BISHOP: {
            for (int sign = 0; sign < 2; sign ++) {
                d.set(1, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, true);
                d.set(-1, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, true);
            }
            break;
        }
        case KNIGHT: {
            for (int sign = 0; sign < 2; sign++) {
                d.set(2, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, false);
                d.set(1, 2); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, false);
                d.set(-2, 1); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, false);
                d.set(-1, 2); if (sign) d.neg();
                genPieceDirectedMove(moves, p, d, false);
            }
            break;
        }
        case PAWN: {
            d.set(0, white ? 1 : -1);
            genPieceDirectedMove(moves, p, d, true);

            int ep = white ? 1 : 0; // Check enemy's en passant status
            if (meta.enPassant[ep] >= 0 && ((white && p.y == 4) || (!white && p.y == 3))) {
                if (p.x + 1 == meta.enPassant[ep] || p.x - 1 == meta.enPassant[ep]) {
                    int y = white ? 5 : 2;
                    moves.push_back(Move(vec2(p.x, p.y), vec2(meta.enPassant[ep], y), ENPASSANT | CAPTURE));
                }
            }
            // Side moves
            d.set(1, d.y);
            genPieceDirectedMove(moves, p, d, false);
            d.set(-1, d.y);
            genPieceDirectedMove(moves, p, d, false);

            break;
        }
        default:
            break;
    }
}

ChessPiece::ChessPiece(ChessPieceType type, bool white) : type(type), white(white) {}

char ChessPiece::toString() {
    switch(type) {
        case KING:
            return white ? 'K' : 'k';
            break;
        case QUEEN:
            return white ? 'Q' : 'q';
            break;
        case CASTLE:
            return white ? 'C' : 'c';
            break;
        case BISHOP:
            return white ? 'B' : 'b';
            break;
        case KNIGHT:
            return white ? 'N' : 'n';
            break;
        case PAWN:
            return white ? 'P' : 'p';
            break;
        default:
            break;
    }
    return ' ';
}

ChessBoard::ChessBoard() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = 0;
        }
    }

    board[0][0] = new ChessPiece(CASTLE, true);
    board[1][0] = new ChessPiece(KNIGHT, true);
    board[2][0] = new ChessPiece(BISHOP, true);
    board[3][0] = new ChessPiece(QUEEN, true);
    board[4][0] = new ChessPiece(KING, true);
    board[5][0] = new ChessPiece(BISHOP, true);
    board[6][0] = new ChessPiece(KNIGHT, true);
    board[7][0] = new ChessPiece(CASTLE, true);

    for (int i = 0; i < 8; i++) {
        board[i][1] = new ChessPiece(PAWN, true);
    }

    board[0][7] = new ChessPiece(CASTLE, false);
    board[1][7] = new ChessPiece(KNIGHT, false);
    board[2][7] = new ChessPiece(BISHOP, false);
    board[3][7] = new ChessPiece(QUEEN, false);
    board[4][7] = new ChessPiece(KING, false);
    board[5][7] = new ChessPiece(BISHOP, false);
    board[6][7] = new ChessPiece(KNIGHT, false);
    board[7][7] = new ChessPiece(CASTLE, false);

    for (int i = 0; i < 8; i++) {
        board[i][6] = new ChessPiece(PAWN, false);
    }

    white = true;
    meta.castling[0][0] = true;
    meta.castling[0][1] = true;
    meta.castling[1][0] = true;
    meta.castling[1][1] = true;

    meta.inCheck[0] = false;
    meta.inCheck[1] = false;

    meta.enPassant[0] = -1;
    meta.enPassant[1] = -1;

    meta.king_pos[0] = vec2(4,0);
    meta.king_pos[1] = vec2(4,7);
}

bool ChessBoard::check_direction(const vec2 king, const bool side,
                                 const vec2 incr, const ChessPieceType TYPE) {
    //cerr << "check direction\n";
    for (vec2 pos = king.add(incr); pos.within(); pos = pos.add(incr)) {
        ChessPiece* piece = getPiece(pos);
        if (piece == nullptr || piece->type == EMPTY) {
            // nothing to see, move on
            //cerr << "empty\n";
            continue;
        }
        if (piece->white == side) {
            // this direction is blocked by own piece
            //cerr << "block\n";
            return false;
            //break;
        }
        if (piece->type == QUEEN || piece->type == TYPE) {
            //cerr << "enemy in check\n";
            return true;
        }
        // enemy in non-threatening direction
        return false;
        //cerr << "unknown " << piece->toString() << std::endl;
    }
    return false;
}

const static vec2 knight_positions[] = {
    vec2(2, 1), vec2(1, 2),
    vec2(-2, 1), vec2(-1, 2),
    vec2(-2, -1), vec2(-1, -2),
    vec2(2, -1), vec2(1, -2)
};

bool ChessBoard::check_knight(const vec2 king, const bool side) {
    for (const vec2& move : knight_positions) {
        vec2 pos = king.add(move);
        if (pos.within()) {
            ChessPiece* piece = getPiece(pos);
            if (piece == nullptr) {
                continue;
            }
            if (piece->type == KNIGHT && piece->white == !side) {
                return true;
            }
        }
    }
    return false;

}
bool ChessBoard::check_pawn(const vec2 king, const bool white) {
    //cerr << "check pawn\n";
    int y = white? king.y + 1 : king.y - 1;
    vec2 pos1(king.x - 1, y);
    if (pos1.within()) {
        ChessPiece* piece = getPiece(pos1);
        if (piece != nullptr && piece->type == PAWN && piece->white == !white) {
            return true;
        }
    }
    vec2 pos2(king.x + 1, y);
    if (pos2.within()) {
        ChessPiece* piece = getPiece(pos2);
        if (piece != nullptr && piece->type == PAWN && piece->white == !white) {
            return true;
        }
    }
    return false;
}

// returns position of enemy king, if within check of this king
bool ChessBoard::check_king(const vec2 king, const bool side) {
    //cerr << "check king\n";
    for (int x = king.x - 1; x <= king.x + 1; x++) {
        for (int y = king.y - 1; y <= king.y + 1; y++) {
            vec2 pos(x, y);
            if (!pos.within()) continue;

            ChessPiece* piece = getPiece(pos);
            if (piece == nullptr) {
                continue;
            }
            if (piece->type == KING && piece->white == !side) {
                return true;
                //return pos;
            }
        }
    }
    return false;
}

bool ChessBoard::isCheck(bool white) {
    bool check(false);

    // find the white king
    vec2 pos = meta.king_pos[white ? 0 : 1];

    check = check_direction(pos, white, vec2(1, 0), CASTLE);
    if (check) return check;
    check = check_direction(pos, white, vec2(-1, 0), CASTLE);
    if (check) return check;
    check = check_direction(pos, white, vec2(0, 1), CASTLE);
    if (check) return check;
    check = check_direction(pos, white, vec2(0, -1), CASTLE);
    if (check) return check;
    check = check_direction(pos, white, vec2(1, 1), BISHOP);
    if (check) return check;
    check = check_direction(pos, white, vec2(1, -1), BISHOP);
    if (check) return check;
    check = check_direction(pos, white, vec2(-1, 1), BISHOP);
    if (check) return check;
    check = check_direction(pos, white, vec2(-1, -1), BISHOP);
    if (check) return check;

    check = check_knight(pos, white);
    if (check) return check;
    check = check_pawn(pos, white);
    if (check) return check;
    check = check_king(pos, white);
    return check;
}

vector<Move> ChessBoard::genMoves() {
    vector<Move> ret;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != NULL && white == board[i][j]->white) {
                genPieceMove(ret, vec2(i, j));
            }
        }
    }

    vector<Move> captureMoves;
    for (Move& m : ret) {
        if (m.moveType & CAPTURE) {
            captureMoves.push_back(m);
        }
    }
    if (captureMoves.size() > 0) {
        ret = captureMoves;
    }

    for (Move& m : ret) {
        valueMove(m);
    }

    if (ret.size() > 1 && ret[0].score > 0) {
        std::sort(ret.begin(), ret.end());
    }

    return ret;
}

void ChessBoard::valueMove(Move& m) {
    int score = 0;
    if (m.moveType & CAPTURE) {
        // ChessPiece* start = board[m.start.x][m.start.y];
        ChessPiece* end = board[m.end.x][m.end.y];

        if (end == 0) { // enPassant
            score += 1; //Pawn
        } else {
            switch(end->type) {
                case PAWN:
                    score += 1;
                    break;
                case KNIGHT:
                    score += 2;
                    break;
                case BISHOP:
                    score += 2;
                    break;
                case CASTLE:
                    score += 3;
                    break;
                case QUEEN:
                    score += 4;
                    break;
                case KING:
                    score += 5;
                    break;
                default:
                    break;
            }
        }

        score = score << 8;
    }

    if (m.moveType & PROMOTION) {
        score += 3;
    }

    if (m.moveType & ENPASSANT) {
        score += 2;
    }

    if (m.moveType & CAPTURE) {
        score += 1;
    }

    m.score = score;
}

void ChessBoard::identifyMoveType(Move& m) {
    short moveType = 0;

    ChessPiece* start = board[m.start.x][m.start.y];
    ChessPiece* end = board[m.end.x][m.end.y];

    if (end == 0) {
        moveType |= QUIET;
    } else {
        moveType |= CAPTURE;
    }

    if (start->type == KING) {
        if (abs(m.start.x - m.end.x) > 1) {
            moveType |= CASTLING;
        }
    } else if (start->type == PAWN) {
        if (end == 0 && m.start.x != m.end.x) { // EnPassant
            moveType |= ENPASSANT;
            moveType |= CAPTURE;
            moveType &= ~QUIET;
        }
        if (m.end.y == 0 || m.end.y == 7) {
            moveType |= PROMOTION;
        }
    }

    m.moveType = moveType;
}

bool ChessBoard::isWhite() const {
    return white;
}

void ChessBoard::setWhite(bool white) {
    this->white = white;
}

ChessPiece* ChessBoard::getPiece(vec2 pos) {
    if (!pos.within()) {
        //cerr << "getPiece nullptr" << endl;
        return nullptr;
    }
    return board[pos.x][pos.y];
}

void ChessBoard::move(Move m) {
    previousMeta.push_back(meta);

    if (m.moveType == 0) {
        identifyMoveType(m);
    }

    ChessPiece* start = board[m.start.x][m.start.y];
    ChessPiece* end = board[m.end.x][m.end.y];
    ChessPieceType removedPieceType = EMPTY;

    int side = white ? 0 : 1;
    meta.enPassant[side] = -1; // reinitialize enpassant

    if (start->type == KING) { // Once king moves, cannot do castling any more
        meta.castling[side][0] = false;
        meta.castling[side][1] = false;

        meta.king_pos[side] = m.end; // Update King's position
    } else if (start->type == CASTLE && (m.start.x == 0 || m.start.x == 7)) {
        // Once castle move, cannot do castling any more
        if ((white && m.start.y == 0) || (!white && m.start.y == 7)) {
            int side_x = m.start.x == 0 ? 0 : 1;
            meta.castling[side][side_x] = false;
        }
    }

    board[m.start.x][m.start.y] = 0;
    if (m.moveType & CAPTURE) { // Capture move
        if (m.moveType & ENPASSANT) { // doing enPassant
            // assert
            ChessPiece* pawn = board[m.end.x][m.start.y]; // neighbor piece

            board[m.end.x][m.start.y] = 0;
            removedPieceType = PAWN;
            delete pawn;
        } else { // Normal capture
            removedPieceType = end->type;
            delete end;
        }
    } else if (m.moveType & CASTLING) { // Castling
        if (m.end.x == 2) {
            board[3][m.start.y] = board[0][m.start.y];
            board[0][m.start.y] = 0;
        } else { // m.end.x == 6
            board[5][m.start.y] = board[7][m.start.y];
            board[7][m.start.y] = 0;
        }
    }

    if (start->type == PAWN) {
        if (abs(m.start.y - m.end.y) > 1) { // allowing for enPassant
            meta.enPassant[side] = m.end.x;
        } else if (m.moveType & PROMOTION) {
            start->type = m.promotion;
        }
    }

    board[m.end.x][m.end.y] = start;

    meta.inCheck[side] = isCheck(white);

    white = !white;

    previousMoves.push_back(UnMove(
        m.start,
        m.end,
        m.moveType,
        removedPieceType));
}

void ChessBoard::unmove() {
    if (previousMoves.size() == 0) {
        return; // Cannot unmove
    }

    meta = previousMeta.back();
    UnMove m = previousMoves.back();
    white = !white;

    ChessPiece* start = board[m.start.x][m.start.y];
    ChessPiece* end = board[m.end.x][m.end.y];

    board[m.end.x][m.end.y] = 0;
    board[m.start.x][m.start.y] = end;

    if (m.moveType & ENPASSANT) {
        int y = m.end.y == 2 ? 3 : 4;
        board[m.end.x][y] = new ChessPiece(m.removed, !white);
    } else if (m.moveType & CAPTURE) {
        board[m.end.x][m.end.y] = new ChessPiece(m.removed, !white);
    } else if (m.moveType & CASTLING) {
        int beginx = m.end.x < m.start.x ? 3 : 5;
        int endx = m.end.x < m.start.x ? 0 : 7;
        board[beginx][m.end.y] = board[endx][m.end.y];
        board[endx][m.end.y] = 0;
    }

    if (m.moveType & PROMOTION) {
        end->type = PAWN; // unpromote
    }

    previousMeta.pop_back();
    previousMoves.pop_back();
}

void ChessBoard::print() {
    for (int j = 7; j >= 0; j--) {
        for (int i = 0; i < 8; i++) {
            ChessPiece* cp = board[i][j];
            if (cp == 0) {
                cout << "-";
            } else {
                cout << cp->toString();
            }
        }
        cout << endl;
    }
}

bool ChessBoard::inCheck(bool white) {
    return meta.inCheck[white ? 0 : 1];
}
