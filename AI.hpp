#pragma once
#ifndef AI_HPP_
#define AI_HPP_

using namespace std;

class AI {
public:
    const int dx[10] = { 1, 1, 0, -1, 0, 1, -1, -1 };
    const int dy[10] = { 1, 0, 1, 1, -1, -1, 0, -1 };

    int lengthToWin = 0;
    int m;
    int n;
    int** board;

    AI(int m, int n, int lengthToWin) {
        this->m = m;
        this->n = n;
        this->lengthToWin = lengthToWin;
        board = new int* [m];
        for (int i = 0; i < m; ++i) {
            board[i] = new int[n];
            for (int j = 0; j < n; ++j) {
                board[i][j] = 0;
            }
        }
    }

    ~AI() {
        for (int i = 0; i < m; ++i) {
            delete[] board[i];
        }
        delete[] board;
    }

    int getBestPosition() {
        pair<int, int> move = { -1, -1 };
        int score = 0, bestScore = -1e8;
        int depth = 4;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (board[i][j] == 0) {
                    board[i][j] = 1;
                    score = minimax(depth, -1e8, 1e8, false);
                    board[i][j] = 0;
                    if (score > bestScore) {
                        bestScore = score;
                        move = { i, j };
                    }
                }
            }
        }

        return move.first * n + move.second;
    }

private:
    int minimax(int depth, int alpha, int beta, bool isMax) {
        int score = evaluate();
        if (depth == 0 || score == 1e8 || score == -1e8) {
            return score;
        }

        if (isMax) {
            int bestScore = -1e8;
            for (int i = 0; i < m; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (board[i][j] == 0) {
                        board[i][j] = 1;
                        bestScore = max(bestScore, minimax(depth - 1, alpha, beta, false));
                        board[i][j] = 0;
                        alpha = max(alpha, bestScore);
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }
            }
            return bestScore;
        }
        else {
            int bestScore = 1e8;
            for (int i = 0; i < m; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (board[i][j] == 0) {
                        board[i][j] = 2;
                        bestScore = min(bestScore, minimax(depth - 1, alpha, beta, true));
                        board[i][j] = 0;
                        beta = min(beta, bestScore);
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }
            }
            return bestScore;
        }
    }

    int evaluate() {
        int score = 0;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (board[i][j] == 1) {
                    for (int k = 0; k < 8; ++k) {
                        int x = i, y = j;
                        int count = 0;
                        while (x >= 0 && x < m && y >= 0 && y < n && board[x][y] == 1) {
                            ++count;
                            x += dx[k];
                            y += dy[k];
                        }
                        if (count == lengthToWin) {
                            return 1e8;
                        }
                        score += count * count;
                    }
                }
                else if (board[i][j] == 2) {
                    for (int k = 0; k < 8; ++k) {
                        int x = i, y = j;
                        int count = 0;
                        while (x >= 0 && x < m && y >= 0 && y < n && board[x][y] == 2) {
                            ++count;
                            x += dx[k];
                            y += dy[k];
                        }
                        if (count == lengthToWin) {
                            return -1e8;
                        }
                        score -= count * count;
                    }
                }
            }
        }
        return score;
    }
};

#endif