#ifndef CONECTFOUR_FUNCTIONAL_H
#define CONECTFOUR_FUNCTIONAL_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>




typedef struct {
    char board[6][7];   // 盤面
    char player;        // プレイヤー記号
    char computer;      // コンピュータ記号
    int turn;           // ターン数 
    char player1Color;
    char player2Color;
    char winColor;
} GameState;

typedef struct {
    int row;
    int col;
} Position;

void animateCoinFlip();
void coinFlipDecisionFirstPlayer();
void resetBoard(GameState *game);
void printBoard(const GameState *game);
int checkFreeSpaces(const GameState *game);
void playerMove(GameState *game);
void computerMove(GameState *game);
char checkWinner(GameState *game);
void printWinner(GameState *game, char winner, int mode);
int evaluateBoard(GameState *game);
int evaluateLine(int compCount, int playerCount);
int minimax(GameState *game, int depth, int alpha, int beta, int maximizingPlayer);
int getDepthForLevel(int comlevel);
void makeBestMove(GameState *game, int comlevel);

#endif