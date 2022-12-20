from chess import Board, Move, square_name, parse_square, piece_symbol, WHITE, BLACK, SQUARES
import time
import sys

class ChessBoard:
    piece_scores = {'p': 1, 'n': 3, 'b': 3, 'r': 5, 'q': 9, 'k': 1000}

    def __init__(self):
        self.board = Board()
        self.__legal_moves__ = None
        self.__legal_moves_refreshed__ = False
        self.__refresh_legal_moves__()

    def is_lose(self):
        return self.board.is_checkmate()

    def is_draw(self):
        return self.board.is_stalemate() or \
               self.board.is_insufficient_material() or \
               self.board.is_fifty_moves() or \
               self.board.is_repetition(3)

    def __is_stalemate__(self):
        return self.board.is_stalemate()

    def get_legal_moves(self):
        if self.__legal_moves_refreshed__:
            return self.__legal_moves__
        self.__eval_legal_moves__()
        return self.__legal_moves__

    def move(self, uci):
        input_move = Move.from_uci(uci)
        if uci not in self.get_legal_moves():
            return False
        self.board.push(input_move)
        self.__refresh_legal_moves__()
        return True

    def pop(self):
        uci = self.board.pop().uci()
        self.__refresh_legal_moves__()
        return uci

    def turn(self):
        return self.board.turn

    def is_white_turn(self):
        return self.board.turn == WHITE

    def __piece_at__(self, uci):
        return self.board.piece_at(parse_square(uci))

    def type_at(self, uci):
        piece = self.__piece_at__(uci)
        if piece is not None:
            return piece_symbol(piece.piece_type)
        return None

    def is_white_at(self, uci):
        piece = self.__piece_at__(uci)
        if piece is not None:
            return piece.color == WHITE
        return None

    def black_score(self):
        return self.__score__(BLACK)

    def white_score(self):
        return self.__score__(WHITE)


    def __score__(self, color):
        score = 0
        for sq in SQUARES:
            piece = self.board.piece_at(sq)
            if piece is not None and piece.color == color:
                score += self.piece_scores[piece_symbol(piece.piece_type)]
        return score

    def __refresh_legal_moves__(self):
        self.__legal_moves_refreshed__ = False


    def __eval_legal_moves__(self):
        self.__legal_moves_refreshed__ = True
        result = [x for x in self.board.legal_moves]
        attack_list = []
        for move in result:
            if self.board.piece_at(move.to_square) is not None:
                attack_list.append(move)
        attack_list = list(filter(lambda x: self.board.gives_check(x), attack_list))
        if attack_list:
            result = attack_list
        self.__legal_moves__ = [x.uci() for x in result]

    def __str__(self):
        return self.board.__str__()

    def __repr__(self):
        return self.board.__repr__()

    def __complete_score__(self):
        piece_score = self.__score__(self.turn()) - self.__score__(not self.turn())
        move_score = 0.1 * len(self.get_legal_moves()) - 1
        final_score = piece_score + move_score
        return final_score

    def minimax(self, depth):
        if self.is_lose():
            return None, -1e5
        if self.is_draw():
            return None, 0
        score = self.__complete_score__()
        if depth <= 0:
            return None, score
        mmax = -1e8
        mmove = None
        scores = []
        candidate_moves = self.get_legal_moves()
        if len(candidate_moves) > depth + 1:
            for move in candidate_moves:
                self.move(move)
                scores.append((self.__complete_score__(), move))
                self.pop()
            scores.sort(key=lambda x: x[0], reverse=True)
            pivot, _ = scores[depth]
            candidate_moves = [m for s, m in filter(lambda s: s[0] >= pivot, scores)]
        if len(candidate_moves) > depth + 2:
            depth -= 1
        for move in candidate_moves:
            self.move(move)
            _, max = self.minimax(depth - 1)
            max = -max
            if max > mmax:
                mmove = move
                mmax = max
            self.pop()
        return mmove, mmax

    def myalg(self, depth):
        if self.is_lose():
            return None, -1e5
        if self.is_draw():
            return None, 0
        score = self.__complete_score__()
        if depth <= 0:
            return None, score
        mmax = -1e8
        mmove = None
        scores = []
        candidate_moves = self.get_legal_moves()
        for move in candidate_moves:
            self.move(move)
            _, max = self.minimax(depth - 1)
            max = -max
            if max > mmax:
                mmove = move
                mmax = max
            self.pop()
        return mmove, mmax

    def breath_alg(self, depth):
        if self.is_lose():
            return None, -1e5
        if self.is_draw():
            return None, 0
        score = self.__complete_score__()
        if depth <= 0:
            return None, score
        mmax = -1e8
        mmove = None
        scores = []
        candidate_moves = self.get_legal_moves()
        for move in candidate_moves:
            self.move(move)
            _, max = self.breath_alg(depth - 1)
            max = -max
            if max > mmax:
                mmove = move
                mmax = max
            self.pop()
        return mmove, mmax
    

def main(argv):
    if len(argv) > 1:
        return 0
    b = ChessBoard()
    if len(argv) == 1:
        if argv[0] == 'white':
            print(b)
            while not b.is_draw() and not b.is_lose():
                step = input('enter your move: ')
                while step not in b.get_legal_moves():
                    step = input('Invalid. Enter your move: ')
                print("player A:")
                b.move(step)
                print(b)
                if b.is_draw():
                    print("game draw, final board: ")
                    break
                elif b.is_lose():
                    print("player B lose, final board: ")
                    print(b)
                    break
                print('--------end of playerA turn--------')
                print("player B:")
                move, _ = b.breath_alg(3)
                if not move:
                    print('no step')
                    break
                b.move(move)
                print(f"playerB move: {move}")
                print(b)
                if b.is_draw():
                    print("game draw, final board: ")
                    break
                elif b.is_lose():
                    print("player A lose, final board: ")
                    print(b)
                    break
                print('--------end of playerB turn----------')
        else:
            while not b.is_draw() and not b.is_lose():
                print("player A:")
                move, _ = b.breath_alg(3)
                if not move:
                    print('no step')
                    break
                b.move(move)
                print(f"playerA move: {move}")
                print(b)
                if b.is_draw():
                    print("game draw, final board: ")
                    break
                elif b.is_lose():
                    print("player B lose, final board: ")
                    print(b)
                    break
                print('--------end of playerA turn----------')
                step = input('enter your move: ')
                while step not in b.get_legal_moves():
                    step = input('Invalid. Enter your move: ')
                print("player B:")    
                b.move(step)
                print(b)
                if b.is_draw():
                    print("game draw, final board: ")
                    break
                elif b.is_lose():
                    print("player A lose, final board: ")
                    print(b)
                    break
                print('--------end of playerB turn--------')
    else:
        while not b.is_draw() and not b.is_lose():
            print("player A:")
            move, _ = b.breath_alg(3)
            if not move:
                print('no step')
                break
            b.move(move)
            print(f"player A move: {move}")
            print(b)
            if b.is_draw():
                print("game draw, final board: ")
                break
            elif b.is_lose():
                print("player B lose, final board: ")
                break
            print('--------end of player A turn--------')
            print('player B:')
            move, _ = b.breath_alg(3)
            if not move:
                print('no step')
                break
            b.move(move)
            print(f"player B move: {move}")
            print(b)
            if b.is_draw():
                print("game draw, final board: ")
                print(b)
                break
            elif b.is_lose():
                print("player A lose, final board: ")
                print(b)
                break
            print('--------end of player B turn----------')
    
    print('ended')



if __name__ == '__main__':
    if len(sys.argv) == 0:
        main([])
    else:
        main(sys.argv[1:])

