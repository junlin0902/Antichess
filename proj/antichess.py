from main import*
import random

def evaluate(board):
    if board.is_white_turn:
        return board.white_score() - board.black_score()
    return board.black_socre() - board.white_score()
        


def alphabeta(board, depth, alpha, beta):
    if (depth < 0):
        return evaluate(board)
    validMove = board.get_legal_moves()
    best = -5000
    turnWhite = board.is_white_turn()
    for m in validMove:
        board.move(m)
        if not board.is_lose() and not board.is_draw():
            temp = -alphabeta(board, depth - 1, -beta, -alpha)
            if temp > best:
                best = temp
                if temp > alpha:
                    alpha = temp
        board.pop()
        if alpha > beta:
            break
    return best


def do(board):
    validMove = board.get_legal_moves()
    bestMove = []
    best = -5000
    alpha = -5000
    for m in validMove:
        temp = -5000
        board.move(m)
        if not board.is_lose() and not board.is_draw():
            temp = -alphabeta(board, 4, -5000, -alpha)
            if temp > best:
                best = temp
                bestMove = []
            if temp > alpha:
                alpha = temp
            if temp == best:
                bestMove.append(m)
        board.pop()
    
    if bestMove == [] or board.is_lose():
        return
    chosenMove = bestMove[random.randint(1, 100) % len(bestMove)]

    ##让总move次数++
    board.move(chosenMove)



board = ChessBoard()
print(board)
while not board.is_lose() and not board.is_draw():
    ##i = input()
    ##while not i in board.get_legal_moves():
        ##i = input()
    ##board.move(i)
    do(board)
    print(board)
    if board.is_lose() or board.is_draw(): ##这里判断游戏有没有结束
        break
    do(board)
    print(board)
