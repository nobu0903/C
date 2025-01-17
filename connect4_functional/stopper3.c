//結果。rate.txtファイルが作成され、モードごとの勝敗数が記録される。

#include "connectfour_functional.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <Windows.h>

void animateCoinFlip() {
    const char *frames[] = {"Heads", "Tails"};
    printf("Flipping the coin...\n");

    // アニメーションのループ
    for (int i = 0; i < 10; i++) {
        printf("\r%s   ", frames[i % 2]); // フレームを切り替える
        fflush(stdout);                  // 画面更新
        Sleep(200);                      // 0.2秒待つ（Windows）
    }
}

void coinFlipDecisionFirstPlayer(int *flipresult) {
    char playerChoice;
    printf("Let's decide the first player by a coin flip!\n");
    printf("Choose Heads (H) or Tails (T): ");
    scanf(" %c", &playerChoice);
    playerChoice = toupper(playerChoice); // 小文字入力も許容する

    if (playerChoice != 'H' && playerChoice != 'T') {
        printf("Invalid choice! Defaulting to Heads.\n");
        playerChoice = 'H';
    }

    animateCoinFlip(); // アニメーションを再生

    int coinResult = rand() % 2; // 0: Heads, 1: Tails
    printf("\r%s!\n", coinResult == 0 ? "Heads" : "Tails");

    // 結果を比較して先攻を決定
    if ((coinResult == 0 && playerChoice == 'H') || (coinResult == 1 && playerChoice == 'T')) {
        *flipresult = 1;
        printf("You won the coin flip! You will go first.\n");
    } else {
        *flipresult = 0;
        printf("You lost the coin flip. The computer will go first.\n");
    }
}

void resetBoard(GameState *game) {
    for(int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            game->board[i][j] = ' ';
        }
    }
}

void printBoard(const GameState *game) {
    printf("\n   1   2   3   4   5   6   7  \n");
    for(int i = 0; i < 6; i++) {
        printf("%d|", i + 1);
        for(int j = 0; j < 7; j++) {
            printf(" %c |", game->board[i][j]);
        }
        printf("\n |---|---|---|---|---|---|---|\n");
    }
}

int checkFreeSpaces(const GameState *game) {
    int freeSpaces = 42;

    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 7; j++) {
            if(game->board[i][j] != ' ') {
                freeSpaces--;
            }
        }
    }
    return freeSpaces;
}

void playerMove(GameState *game) {
    int x;

    do {
        printf("Enter column #(1-7): ");
        scanf("%d", &x);
        x--;

        if (x < 0 || x > 6 || game->board[0][x] != ' ') {
            printf("Invalid column. Try again.\n");
        }
    } while (x < 0 || x > 6 || game->board[0][x] != ' ');

    for(int i = 5; i >= 0; i--) {
        if(game->board[i][x] == ' ') {
            game->board[i][x] = game->player;
            break;
        }
    }
}

void secondPlayerMove(GameState *game) {
    int x;

    do {
        printf("Enter column #(1-7): ");
        scanf("%d", &x);
        x--;

        if (x < 0 || x > 6 || game->board[0][x] != ' ') {
            printf("Invalid column. Try again.\n");
        }
    } while (x < 0 || x > 6 || game->board[0][x] != ' ');

    for(int i = 5; i >= 0; i--) {
        if(game->board[i][x] == ' ') {
            game->board[i][x] = game->computer; // second player uses the computer symbol
            break;
        }
    }
}

void computerMove(GameState *game) {
    int bestScore = INT_MIN;
    int bestCol = 0;

    // Evaluate each column and select the one with the highest score
    for(int col = 0; col < 7; col++) {
        if (game->board[0][col] == ' ') { // Evaluate only empty columns
            // Place a piece on the temporary board
            for(int row = 5; row >= 0; row--) {
                if (game->board[row][col] == ' ') {
                    game->board[row][col] = game->computer;
                    int score = minimax(game, 5, INT_MIN, INT_MAX, 0); // Look ahead 5 moves
                    game->board[row][col] = ' '; // Reset the piece

                    if (score > bestScore) {
                        bestScore = score;
                        bestCol = col;
                    }
                    break;
                }
            }
        }
    }

    // Place a piece in the best column
    for(int row = 5; row >= 0; row--) {
        if(game->board[row][bestCol] == ' ') {
            game->board[row][bestCol] = game->computer;
            break;
        }
    }
}

// Function to evaluate the board
int evaluateBoard(GameState *game) {
    int score = 0;

    // Check horizontal lines
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 4; col++) {
            int compCount = 0;
            int playerCount = 0;
            
            for (int i = 0; i < 4; i++) {
                if (game->board[row][col + i] == game->computer) compCount++;
                if (game->board[row][col + i] == game->player) playerCount++;
            }

            score += evaluateLine(compCount, playerCount);
        }
    }

    // Check vertical lines
    for (int col = 0; col < 7; col++) {
        for (int row = 0; row < 3; row++) {
            int compCount = 0;
            int playerCount = 0;
            
            for (int i = 0; i < 4; i++) {
                if (game->board[row + i][col] == game->computer) compCount++;
                if (game->board[row + i][col] == game->player) playerCount++;
            }

            score += evaluateLine(compCount, playerCount);
        }
    }

    // Check diagonal lines (\ direction)
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            int compCount = 0;
            int playerCount = 0;
            
            for (int i = 0; i < 4; i++) {
                if (game->board[row + i][col + i] == game->computer) compCount++;
                if (game->board[row + i][col + i] == game->player) playerCount++;
            }

            score += evaluateLine(compCount, playerCount);
        }
    }

    // Check diagonal lines (/ direction)
    for (int row = 3; row < 6; row++) {
        for (int col = 0; col < 4; col++) {
            int compCount = 0;
            int playerCount = 0;
            
            for (int i = 0; i < 4; i++) {
                if (game->board[row - i][col + i] == game->computer) compCount++;
                if (game->board[row - i][col + i] == game->player) playerCount++;
            }

            score += evaluateLine(compCount, playerCount);
        }
    }

    return score;
}

// 評価スコアを返す補助関数
int evaluateLine(int compCount, int playerCount) {
    if (compCount > 0 && playerCount > 0) return 0; // 両方のコマがある場合は無効
    if (compCount == 3) return 50;
    if (compCount == 2) return 10;
    if (playerCount == 3) return -50;
    if (playerCount == 2) return -10;
    return 0;
}

// Minimax function to search for the best move
int minimax(GameState *game, int depth, int alpha, int beta, int maximizingPlayer) {
    char winner = checkWinner(game);
    if (winner == game->computer)
        return 1000; // Score for a win
    else if (winner == game->player)
        return -1000; // Score for a loss
    else if (depth == 0 || checkFreeSpaces(game) == 0)
        return evaluateBoard(game); // Score when depth limit or draw is reached

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (int col = 0; col < 7; col++) {
            if (game->board[0][col] == ' ') {
                for (int row = 5; row >= 0; row--) {
                    if (game->board[row][col] == ' ') {
                        game->board[row][col] = game->computer;
                        int eval = minimax(game, depth - 1, alpha, beta, 0);
                        game->board[row][col] = ' ';
                        maxEval = maxEval > eval ? maxEval : eval;
                        alpha = alpha > eval ? alpha : eval;
                        if (beta <= alpha) break;
                        break;
                    }
                }
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int col = 0; col < 7; col++) {
            if (game->board[0][col] == ' ') {
                for (int row = 5; row >= 0; row--) {
                    if (game->board[row][col] == ' ') {
                        game->board[row][col] = game->player;
                        int eval = minimax(game, depth - 1, alpha, beta, 1);
                        game->board[row][col] = ' ';
                        minEval = minEval < eval ? minEval : eval;
                        beta = beta < eval ? beta : eval;
                        if (beta <= alpha) break;
                        break;
                    }
                }
            }
        }
        return minEval;
    }
}

// Check who connected four in a row
// Check who connected four in a row
char checkWinner(GameState *game) {
    // Check vertical line
    for(int j = 0; j < 7; j++) {
        for(int i = 0; i < 3; i++) {
            if(game->board[i][j] == game->board[i+1][j] && game->board[i+1][j] == game->board[i+2][j] && game->board[i+2][j] == game->board[i+3][j] && game->board[i][j] != ' ') {
                return game->board[i][j];
            }
        }
    }

    // Check horizontal line
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 4; j++) {
            if(game->board[i][j] == game->board[i][j+1] && game->board[i][j+1] == game->board[i][j+2] && game->board[i][j+2] == game->board[i][j+3] && game->board[i][j] != ' ') {
                return game->board[i][j];
            }
        }
    }

    // Check diagonal (bottom-left to top-right)
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 4; j++) {
            if(game->board[i][j] == game->board[i+1][j+1] && game->board[i+1][j+1] == game->board[i+2][j+2] && game->board[i+2][j+2] == game->board[i+3][j+3] && game->board[i][j] != ' ') {
                return game->board[i][j];
            }
        }
    }

    // Check diagonal (top-left to bottom-right)
    for(int i = 3; i < 6; i++) {
        for(int j = 0; j < 4; j++) {
            if(game->board[i][j] == game->board[i-1][j+1] && game->board[i-1][j+1] == game->board[i-2][j+2] && game->board[i-2][j+2] == game->board[i-3][j+3] && game->board[i][j] != ' ') {
                return game->board[i][j];
            }
        }
    }

    return ' ';
}

// Print winner
void printWinner(GameState *game, char winner, int mode) {
    if (mode == 1) { // Play with friend
        if (winner == game->player) {
            printf("Player 1 wins!\n");
        } else if (winner == game->computer) {
            printf("Player 2 wins!\n");
        } else {
            printf("It's a tie!\n");
        }
    } else { // Play with computer
        if (winner == game->player) {
            printf("You win!\n");
        } else if (winner == game->computer) {
            printf("You lose!\n");
        } else {
            printf("It's a tie!\n");
        }
    }

    // 勝利部分を黄色で塗る
    if (winner != ' ') {
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 7; j++) {
                if (game->board[i][j] == winner) {
                    game->board[i][j] = game->winColor;
                }
            }
        }
    }
}

// Update the rate file with the result

void updateRateFile(int mode, char winner) {
    FILE *file = fopen("rate.txt", "r+");
    if (!file) {
        file = fopen("rate.txt", "w");
        fprintf(file, "|mode   | WIN | LOSS|\n");
        fprintf(file, "|-------------------|\n");
        fprintf(file, "|easy   |  0  |  0  |\n");
        fprintf(file, "|-------------------|\n");
        fprintf(file, "|normal |  0  |  0  |\n");
        fprintf(file, "|-------------------|\n");
        fprintf(file, "|hard   |  0  |  0  |\n");
        fprintf(file, "|-------------------|\n");
        fclose(file);
        file = fopen("rate.txt", "r+");
    }

    char buffer[256];
    int win[3] = {0, 0, 0};
    int loss[3] = {0, 0, 0};

    // Read current values
    while (fgets(buffer, sizeof(buffer), file)) {
        if (sscanf(buffer, "|easy   |  %d  |  %d  |", &win[0], &loss[0]) == 2) continue;
        if (sscanf(buffer, "|normal |  %d  |  %d  |", &win[1], &loss[1]) == 2) continue;
        if (sscanf(buffer, "|hard   |  %d  |  %d  |", &win[2], &loss[2]) == 2) continue;
    }

    // Update values based on mode and winner
    if (winner == 'X') { // プレイヤーが勝った場合
        win[mode]++;
    } else if (winner == 'O') { // コンピューターが勝った場合
        loss[mode]++;
    }

    // Write updated values
    fseek(file, 0, SEEK_SET);
    fprintf(file, "|mode   | WIN | LOSS|\n");
    fprintf(file, "|-------------------|\n");
    fprintf(file, "|easy   |  %d  |  %d  |\n", win[0], loss[0]);
    fprintf(file, "|-------------------|\n");
    fprintf(file, "|normal |  %d  |  %d  |\n", win[1], loss[1]);
    fprintf(file, "|-------------------|\n");
    fprintf(file, "|hard   |  %d  |  %d  |\n", win[2], loss[2]);
    fprintf(file, "|-------------------|\n");

    fclose(file);
}

int getDepthForLevel(int comlevel) {
    switch(comlevel) {
        case 1: return 1; // easy
        case 2: return 3; // normal
        case 3: return 5; // hard
        default: return 3; // デフォルトはnormal
    }
}

void makeBestMove(GameState *game, int comlevel) {
    int depth = getDepthForLevel(comlevel); // レベルに応じた深さを設定
    int bestMove = -1;
    int bestValue = INT_MIN;

    for (int col = 0; col < 7; col++) {
        if (game->board[0][col] == ' ') {
            for (int row = 5; row >= 0; row--) {
                if (game->board[row][col] == ' ') {
                    game->board[row][col] = game->computer;
                    int moveValue = minimax(game, depth, INT_MIN, INT_MAX, 0);
                    game->board[row][col] = ' ';
                    if (moveValue > bestValue) {
                        bestMove = col;
                        bestValue = moveValue;
                    }
                    break;
                }
            }
        }
    }

    if (bestMove != -1) {
        for (int row = 5; row >= 0; row--) {
            if (game->board[row][bestMove] == ' ') {
                game->board[row][bestMove] = game->computer;
                break;
            }
        }
    }
}

int main () {
    GameState game;
    char winner = ' ';
    resetBoard(&game);
    int flipresult;

    int mode;
    printf("Hi! Let's play connectfour!\n");

    while(1) {
        printf("Choose mode!\n"
            "1. Play with friend.\n"
            "2. Play with computer\n");
        scanf("%d", &mode);

        switch (mode) {
            case 1:
                printf("You chose Play with Friend\n");
                coinFlipDecisionFirstPlayer(&flipresult);
                resetBoard(&game);
                game.player = 'O'; // プレイヤー1の駒を設定
                game.computer = 'X'; // プレイヤー2の駒を設定

                while (winner == ' ' && checkFreeSpaces(&game) != 0) {
                    printBoard(&game);
                    playerMove(&game);
                    winner = checkWinner(&game);
                    if (winner != ' ' || checkFreeSpaces(&game) == 0) {
                        break;
                    }

                    printBoard(&game);
                    secondPlayerMove(&game);
                    winner = checkWinner(&game);
                    if (winner != ' ' || checkFreeSpaces(&game) == 0) {
                        break;
                    }
                }
                break;
            case 2:
                printf("You chose Play with Computer\n");
                printf("Which level computer do you want in 1 to 3?\n"
                        "1. Easy\n"
                        "2. Normal\n"
                        "3. Hard\n");
                int comlevel;
                scanf("%d", &comlevel);
                if (comlevel >= 1 && comlevel <= 3) {
                    makeBestMove(&game, comlevel);
                } else {
                    printf("Invalid level. Defaulting to Normal.\n");
                    comlevel = 2;
                    makeBestMove(&game, comlevel);
                }
                printf("You chose level %d computer. Let's decide the first player by a coin flip!\n", comlevel);
                coinFlipDecisionFirstPlayer(&flipresult);
                resetBoard(&game);
                game.player = 'O'; // プレイヤーの駒を設定
                game.computer = 'X'; // コンピュータの駒を設定

                while (winner == ' ' && checkFreeSpaces(&game) != 0) {
                    printBoard(&game);
                    if (flipresult == 1) {
                        playerMove(&game);
                        winner = checkWinner(&game);
                        if (winner != ' ' || checkFreeSpaces(&game) == 0) {
                            break;
                        }

                        printBoard(&game);
                        computerMove(&game);
                        winner = checkWinner(&game);
                        if (winner != ' ' || checkFreeSpaces(&game) == 0) {
                            break;
                        }
                    } else {
                        computerMove(&game);
                        winner = checkWinner(&game);
                        if (winner != ' ' || checkFreeSpaces(&game) == 0) {
                            break;
                        }

                        printBoard(&game);
                        playerMove(&game);
                        winner = checkWinner(&game);
                        if (winner != ' ' || checkFreeSpaces(&game) == 0) {
                            break;
                        }
                    }
                }
                updateRateFile(comlevel, winner); // 試合終了後にレートを更新
                break;
            default:
                printf("Invalid mode. Please choose again.\n");
                break;
        }
        break; // モード選択後にループを抜ける
    }

    // 最終結果を表示
    printBoard(&game);
    printWinner(&game, winner, mode);

    return 0;
}