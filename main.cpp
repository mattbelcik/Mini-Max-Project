#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <time.h>

using namespace std;

#define MAXLEVEL 2
#define MINSCORE -1000
#define MAXSCORE 1000
float evaluate(char[4][4], char);

/* Functions to play TicTicTacToe game */
int countThreesInARow(char[4][4], char);
bool fourInARow(char[4][4], char);
void display(char[4][4]);
int run(int[2], char);

/* Functions to run minmax tree */
void choose(int &, int &, char[4][4], char);
float getmax(int &, int &, char[4][4], int, char);
float getmin(int &, int &, char[4][4], int, char);
void copy(char[4][4], char[4][4]);

/****************** YOUR CODE GOES HERE ****************************/

/* User provided function to evaluate how good a situation is */

float evaluate(char board[4][4])
{
    float score = 0;
    // Initialize weights with a focus on central control
    int weights[4][4] = {
        {1, 3, 3, 1},
        {3, 5, 5, 3},
        {3, 5, 5, 3},
        {1, 3, 3, 1}};

    // Calculate filled squares to adjust strategy dynamically
    int filledSquares = 0;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (board[i][j] != ' ')
                filledSquares++;
        }
    }
    // Early game: Boost central control
    if (filledSquares < 8)
    {
        weights[1][1] = weights[1][2] = weights[2][1] = weights[2][2] = 10;
    }

    // Dynamic Adjustment Based on Game State
    // Increase emphasis on blocking and creating multiple win conditions in mid to late game
    if (filledSquares >= 8)
    {
        // Adjusting weights dynamically to respond to the evolving game state
        // This includes prioritizing blocking opponent wins and setting up future wins
        int threesO = countThreesInARow(board, 'O');
        int threesX = countThreesInARow(board, 'X');
        if (threesO > threesX)
        {
            score += threesO * 20; // Increase score if AI ('O') has more threes
        }
        else if (threesX > 0)
        {
            score -= threesX * 30; // Decrease score significantly to prioritize blocking
        }
    }

    // Apply weights to score based on current board state, rewarding or penalizing positions based on their strategic value.
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (board[i][j] == 'O')
                score += weights[i][j];
            else if (board[i][j] == 'X')
                score -= weights[i][j];
        }
    }

    // Immediate Win or Loss Conditions
    if (fourInARow(board, 'O'))
        score += 1000; // Significant score boost for a win
    if (fourInARow(board, 'X'))
        score -= 1000; // Significant score reduction for opponent's win potential

    // Adjust score for potential wins (threes in a row)
    score += countThreesInARow(board, 'O') * 50; // Favor having 'O' threes
    score -= countThreesInARow(board, 'X') * 50; // Disfavor 'X' threes

    // Ensuring score stays within defined limits
    score = min(score, 1000.0f);
    score = max(score, -1000.0f);

    return score;
}

/***************** END OF YOUR CODE ********************************/

/* Below are some VERY useful utility functions designed to make your life easier.
   They are used by the game software to detect 4-in-a-rows and count 3-in-a-rows,
   but you might find them very useful for your own heuristics. */

/* This function determines whether who (either 'X' or 'O' has 4 in a row (and therefore
   has won the game).
   Hint: It is a very good thing for the AI if fourInARow(board, 'O') is true, and very
   bad for the AI if fourInARow(board, 'X') is true! */

bool fourInARow(char board[4][4], char who)
{
    for (int i = 0; i < 4; i++)
    {
        if (board[i][0] == who &&
            board[i][1] == who &&
            board[i][2] == who &&
            board[i][3] == who)
        {
            return true;
        }
    }
    for (int i = 0; i < 4; i++)
    {
        if (board[0][i] == who &&
            board[1][i] == who &&
            board[2][i] == who &&
            board[3][i] == who)
        {
            return true;
        }
    }
    if (board[0][0] == who &&
        board[1][1] == who &&
        board[2][2] == who &&
        board[3][3] == who)
    {
        return true;
    }

    if (board[0][3] == who &&
        board[1][2] == who &&
        board[2][1] == who &&
        board[3][0] == who)
    {
        // cout << "Win along other diagonal\n";
        return true;
    }
    return 0;
}

/* This function exhaustively searches the current board to count the
   sets of 3 that player who has in a row.
   Hint: You might find it very useful to compare the number of threes in a row the player
   and AI have for a given board. */

int countThreesInARow(char board[4][4], char who)
{
    int row, col;
    int score = 0;

    /* check all rows */
    for (row = 0; row < 4; row++)
        for (col = 0; col < 2; col++)
            if (board[row][col] == who &&
                board[row][col + 1] == who &&
                board[row][col + 2] == who)
                score++;

    /* check all columns */
    for (row = 0; row < 2; row++)
        for (col = 0; col < 4; col++)
            if (board[row][col] == who &&
                board[row + 1][col] == who &&
                board[row + 2][col] == who)
                score++;

    /* check all diagonals */
    for (row = 0; row < 2; row++)
        for (col = 0; col < 2; col++)
            if (board[row][col] == who &&
                board[row + 1][col + 1] == who &&
                board[row + 2][col + 2] == who)
                score++;
    for (row = 0; row < 2; row++)
        for (col = 2; col < 4; col++)
            if (board[row][col] == who &&
                board[row + 1][col - 1] == who &&
                board[row + 2][col - 2] == who)
                score++;

    return score;
}

/* This is the main function for playing the game. It alternatively
   prompts the user for a move, and uses the minmax algorithm in
   conjunction with the given evaluation function to determine the
   opposing move. This continues until the board is full. It returns
   the number scored by X minus the number scored by O. */

int run(int scores[2], char who)
{
    int i, j;
    char board[4][4];
    int row, col;
    char current, other;
    int move = 1;

    /* Initialize the board */
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            board[i][j] = ' ';
        }
    }

    if (who == 'O')
        display(board);

    while (move <= 16)
    {
        if (move % 2 == 1)
        {
            current = 'X';
            other = 'O';
        }
        else
        {
            current = 'O';
            other = 'X';
        }

        if (current == who)
        {                                 /* The computer's move */
            choose(row, col, board, who); /* Call function to compute move */
            cout << "Computer chooses " << row + 1 << ", " << col + 1 << endl;
            if (board[row][col] == ' ')
                board[row][col] = current;
            else
            {
                cout << "BUG! " << row + 1 << ", " << col + 1 << " OCCUPIED!!!\n";
                exit(0);
            }
            if (fourInARow(board, who))
            {
                cout << "Computer has 4 in a row! Computer wins!";
                display(board);
                return 1;
            }
        }

        else
        {
            cout << "Player " << current << ", enter your move: ";
            cin >> row >> col;
            while (board[row - 1][col - 1] != ' ' ||
                   row < 1 || row > 4 ||
                   col < 1 || col > 4)
            {
                cout << "Illegal move! You cannot use that square!\n";
                cin >> row >> col;
            }
            col--;
            row--;
            board[row][col] = current;
            if (fourInARow(board, current))
            {
                cout << "Player has 4 in a row! Player wins!";
                display(board);
                return 1;
            }
        }

        display(board); /* Redisplay board to show the move */

        move++; /* Increment the move number and do next move. */
    }
    scores[0] = countThreesInARow(board, 'X');
    scores[1] = countThreesInARow(board, 'O');
    return 0;
}

/* This displays the current configuration of the board. */

void display(char board[4][4])
{
    int row, col, X, O;
    cout << "\n";
    for (row = 3; row >= 0; row--)
    {
        cout << "  +-+-+-+-+\n";
        cout << row + 1 << " ";
        for (col = 0; col < 4; col++)
        {
            if (board[row][col] == 'X') /* if contents are 0, print space */
                cout << "|X";
            else if (board[row][col] == 'O')
                cout << "|0";
            else
                cout << "| ";
        }
        cout << "|\n";
    }
    cout << "  +-+-+-+-+\n"; /* print base, and indices */
    cout << "   1 2 3 4\n";
    cout << "X: " << countThreesInARow(board, 'X') << endl;
    cout << "O: " << countThreesInARow(board, 'O') << endl;
}

/* Basic function for choosing the computer's move. It essentially
   initiates the first level of the MINMAX algorithm, and returns
   the column number it chooses. */

void choose(int &row, int &col, char board[4][4], char who)
{
    int move;
    float value;
    getmax(row, col, board, 1, who);
}

/* This handles any MAX level of a MINMAX tree. Using a pointer to move
   is a hack to deal with not being able to return both the chosen move
   and its evaluation number.

   This essentially handles moves for the computer. */

float getmax(int &row, int &col, char board[4][4], int level, char who)
{
    char tempboard[4][4];
    int r, c = 0;
    float max = MINSCORE - 1;
    float val;
    int tempR, tempC;
    for (r = 0; r < 4; r++)
        for (c = 0; c < 4; c++)
        { /* Try each row and column in board */
            if (board[r][c] == ' ')
            { /* Make sure square not full */

                /* To avoid changing original board  during tests, make a copy */
                copy(tempboard, board);

                /* Find out what would happen if we chose this column */
                tempboard[r][c] = who;

                /* If this is the bottom of the search tree (that is, a leaf) we need
                   to use the evaluation function to decide how good the move is */
                if (level == MAXLEVEL)
                {
                    val = evaluate(tempboard);
                }

                /* Otherwise, this move is only as good as the worst thing our
                   opponent can do to us. */
                else
                    val = getmin(tempR, tempC, tempboard, level + 1, who);

                /* Return the highest evaluation, and set call by ref. parameter
                   "move" to the corresponding column */
                if (val > max)
                {
                    max = val;
                    if (level == 1)
                    {
                        row = r;
                        col = c;
                    }
                }
            }
        }
    return max;
}

/* This handles any MIN level of a MINMAX tree. Using a pointer to move
   is a hack to deal with not being able to return both the chosen move
   and its evaluation number.

   This essentially handles moves for the opponent. */

float getmin(int &row, int &col, char board[4][4], int level, char who)
{
    char tempboard[4][4];
    int r, c = 0;
    int tempR, tempC;
    float min = MAXSCORE + 1;
    float val;

    /* Since this is opponent's move, we need to figure out which they are */
    char other;
    if (who == 'X')
        other = 'O';
    else
        other = 'X';

    for (r = 0; r < 4; r++)
        for (c = 0; c < 4; c++)
        { /* Try each row and column in board */
            if (board[r][c] == ' ')
            { /* Make sure square not full */

                /* To avoid changing original board  during tests, make a copy */
                copy(tempboard, board);

                /* Find out what would happen if opponent chose this column */
                tempboard[r][c] = other;

                /* If this is the bottom of the search tree (that is, a leaf) we need
                   to use the evaluation function to decide how good the move is */
                if (level == MAXLEVEL)
                    val = evaluate(tempboard);

                /* Otherwise, find the best thing that we can do if opponent
                   chooses this move. */
                else
                    val = getmax(tempR, tempC, tempboard, level + 1, who);

                /* Return the lowest evaluation (which we will assume will be
                   chosen by opponent, and set call by ref. parameter
                   "move" to the corresponding column */
                if (val < min)
                {
                    min = val;
                    // *move = col;
                }
            }
        }
    return min;
}

/* This function makes a copy of a given board. This is necessary to be
   able to "try out" the effects of different moves without messing up
   the actual current board. */

void copy(char a[4][4], char b[4][4])
{
    int i, j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            a[i][j] = b[i][j];
        }
    }
}

int main(int argc, char **argv)
{
    char player, computer;
    char line[5];
    int scores[2];
    /* Modified for simplicity. The player is always X, and the AI is always O. */

    int win = 0;
    win = run(scores, 'O');
    if (!win)
        cout << "\nFinal score: \nX: " << scores[0] << "\nO: " << scores[1] << "\n";

    return 1;
}
