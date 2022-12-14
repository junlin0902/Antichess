#ifndef _BOARD_H_
#define _BOARD_H_
#include <vector>
#include <string>

struct vec2 {
    short x;
    short y;

    vec2();
    vec2(short x, short y);
    void set(short x, short y);
    vec2 add(const vec2& v) const;
    void neg();

    bool within() const;
};

enum ChessPieceType {
    KING,
    QUEEN,
    CASTLE,
    BISHOP,
    KNIGHT,
    PAWN,
    EMPTY
};

#define QUIET 0x1
#define CAPTURE 0x2
#define CASTLING 0x4
#define ENPASSANT 0x8
#define PROMOTION 0x10

struct Move {
    vec2 start;
    vec2 end;

    // BitMap
    // 0 : Unknown
    // 1 : Quiet
    // 2 : Capture
    // 3 : Castling
    // 4 : EnPassant
    // 5 : Promotion
    short moveType;
    int score;

    ChessPieceType promotion;

    Move(vec2 start, vec2 end);
    Move(vec2 start, vec2 end, short moveType);

    std::string toString();
};

bool operator<(Move const & a, Move const & b);

struct UnMove {
    vec2 start;
    vec2 end;
    short moveType;

    ChessPieceType removed;
    UnMove(vec2 start, vec2 end, short moveType, ChessPieceType removedPiece);
};

struct ChessPiece {
    ChessPieceType type;
    bool white;

    ChessPiece(ChessPieceType type, bool white);

    char toString();
};

struct MetaChessBoard {
    bool castling[2][2];
    bool inCheck[2];
    short enPassant[2];
    vec2 king_pos[2];
};

class ChessBoard {
public:
    ChessBoard();
    std::vector<Move> genMoves();
    void move(Move m);
    void unmove();
    void print();

    void identifyMoveType(Move& m);
    void valueMove(Move& m);

    bool isWhite() const;
    void setWhite(bool white);

    bool inCheck(bool white);

    ChessPiece* getPiece(vec2 pos);
private:
    bool isCheck(bool white);
    void genPieceMove(std::vector<Move>& moves, vec2 p);
    void genPieceDirectedMove(std::vector<Move>& moves, vec2 from, vec2 d, bool multi);

    // functions to detect whether board is in check
    bool check_direction(const vec2 king, const bool side,
                         const vec2 incr, const ChessPieceType TYPE);
    bool check_knight(const vec2 king, const bool side);
    bool check_pawn(const vec2 king, const bool side);
    bool check_king(const vec2 king, const bool side);

    bool white;
    MetaChessBoard meta;
    ChessPiece* board[8][8];

    std::vector<UnMove> previousMoves;
    std::vector<MetaChessBoard> previousMeta;
};

#endif
