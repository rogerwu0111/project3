#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"

using namespace std;
# define INFINITY 9999999
# define MAX_DEPTH 4

/******************************************************
 * In your algorithm, you can just use the the funcitons
 * listed by TA to get the board information.(functions 
 * 1. ~ 4. are listed in next block)
 * 
 * The STL library functions is not allowed to use.
******************************************************/

/*************************************************************************
 * 1. int board.get_orbs_num(int row_index, int col_index)
 * 2. int board.get_capacity(int row_index, int col_index)
 * 3. char board.get_cell_color(int row_index, int col_index)
 * 4. void board.print_current_board(int row_index, int col_index, int round)
 * 
 * 1. The function that return the number of orbs in cell(row, col)
 * 2. The function that return the orb capacity of the cell(row, col)
 * 3. The function that return the color fo the cell(row, col)
 * 4. The function that print out the current board statement
*************************************************************************/

int my_evaluate(Board board, Player player)
{
    char my_color, enemy_color;
    if (player.get_color() == RED)
    {
        my_color = RED;
        enemy_color = BLUE;
    }
    else
    {
        my_color = BLUE;
        enemy_color = RED;
    }
    
    int i, j;
    int point = 0;
    // my orb minus enemy orb with weighting
    for (i = 0; i<ROW; i++)
    {
        for (j = 0; j<COL; j++)
        {
            if (board.get_cell_color(i, j) == my_color)
            {
                point += 4-board.get_capacity(i, j);
            }
            else if (board.get_cell_color(i, j) == enemy_color)
            {
                point -= 4-board.get_capacity(i, j);
            }
        }
    }

    // check "absolute" position
    int adjROW, adjCOL, current_num, current_capacity, adj_num, adj_capacity;
    bool IsAbsolute;
    // my orb
    for (i = 0; i<ROW; i++)
    {
        for (j = 0; j<COL; j++)
        {
            if (board.get_cell_color(i, j) == my_color)
            {
                current_num = board.get_orbs_num(i, j);
                current_capacity = board.get_capacity(i, j);
                IsAbsolute = true;

                adjROW = i+1;
                adjCOL = j;
                adj_num = board.get_orbs_num(adjROW, adjCOL);
                adj_capacity = board.get_capacity(adjROW, adjCOL);
                if (adjROW < ROW)
                {
                    if (board.get_cell_color(adjROW, adjCOL) == enemy_color &&
                    adj_capacity-adj_num <= current_capacity-current_num)
                    {
                        IsAbsolute = false;
                    }
                }

                adjROW = i-1;
                adjCOL = j;
                adj_num = board.get_orbs_num(adjROW, adjCOL);
                adj_capacity = board.get_capacity(adjROW, adjCOL);
                if (adjROW >= 0)
                {
                    if (board.get_cell_color(adjROW, adjCOL) == enemy_color &&
                    adj_capacity-adj_num <= current_capacity-current_num)
                    {
                        IsAbsolute = false;
                    }
                }

                adjROW = i;
                adjCOL = j+1;
                adj_num = board.get_orbs_num(adjROW, adjCOL);
                adj_capacity = board.get_capacity(adjROW, adjCOL);
                if (adjCOL < COL)
                {
                    if (board.get_cell_color(adjROW, adjCOL) == enemy_color &&
                    adj_capacity-adj_num <= current_capacity-current_num)
                    {
                        IsAbsolute = false;
                    }
                }

                adjROW = i;
                adjCOL = j-1;
                adj_num = board.get_orbs_num(adjROW, adjCOL);
                adj_capacity = board.get_capacity(adjROW, adjCOL);
                if (adjCOL >= 0)
                {
                    if (board.get_cell_color(adjROW, adjCOL) == enemy_color &&
                    adj_capacity-adj_num <= current_capacity-current_num)
                    {
                        IsAbsolute = false;
                    }
                }

                if (IsAbsolute) point++;
            }
        }
    }

    // enemy orb
    for (i=0; i<ROW; i++)
    {
        for (j = 0; j<COL; j++)
        {
            if (board.get_cell_color(i, j) == enemy_color)
            {
                current_num = board.get_orbs_num(i, j);
                current_capacity = board.get_capacity(i, j);
                IsAbsolute = true;

                adjROW = i+1;
                adjCOL = j;
                adj_num = board.get_orbs_num(i, j);
                adj_capacity = board.get_capacity(i, j);
                if (adjROW < ROW)
                {
                    if (board.get_cell_color(adjROW, adjCOL) == my_color &&
                    adj_capacity-adj_num <= current_capacity-current_num)
                    {
                        IsAbsolute = false;
                    }
                }

                adjROW = i-1;
                adjCOL = j;
                adj_num = board.get_orbs_num(i, j);
                adj_capacity = board.get_capacity(i, j);
                if (adjROW >= 0)
                {
                    if (board.get_cell_color(adjROW, adjCOL) == my_color &&
                    adj_capacity-adj_num <= current_capacity-current_num)
                    {
                        IsAbsolute = false;
                    }
                }

                adjROW = i;
                adjCOL = j+1;
                adj_num = board.get_orbs_num(i, j);
                adj_capacity = board.get_capacity(i, j);
                if (adjCOL < COL)
                {
                    if (board.get_cell_color(adjROW, adjCOL) == my_color &&
                    adj_capacity-adj_num <= current_capacity-current_num)
                    {
                        IsAbsolute = false;
                    }
                }
                
                adjROW = i;
                adjCOL = j-1;
                adj_num = board.get_orbs_num(i, j);
                adj_capacity = board.get_capacity(i, j);
                if (adjCOL >= 0)
                {
                    if (board.get_cell_color(adjROW, adjCOL) == my_color &&
                    adj_capacity-adj_num <= current_capacity-current_num)
                    {
                        IsAbsolute = false;
                    }
                }

                if (IsAbsolute) point++;
            }
        }
    }
    return point;
}

int min_max_algorithm(Board board, int depth, int alpha, int beta, bool IsMaxLevel, int index[], Player player)
{
    Board copy_board;

    if (depth == 0)
    {
        Player me = player;
        copy_board = board;
        return my_evaluate(copy_board, me);
    }

    int i, j, eval;
    if (IsMaxLevel)
    {
        int max = -INFINITY;
        Player me = player;
        for (i = 0; i<ROW; i++)
        {
            for (j = 0; j<COL; j++)
            {
                copy_board = board;
                me = player;
                if (copy_board.place_orb(i, j, &me))
                {
                    eval = min_max_algorithm(copy_board, depth-1, alpha, beta, false, index, player);
                    if (eval > max) 
                    {
                        max = eval;
                        // update
                        if (depth == MAX_DEPTH)
                        {
                            index[0] = i;
                            index[1] = j;
                        }
                    }
                    if (max > alpha) alpha = max;
                    if (alpha >= beta) break;
                }
            }
            if (j < COL) break;
        }
        return max;
    }

    else
    {
        int min = INFINITY;
        // construct opponent
        char color = player.get_color();
        if (color == RED) color = BLUE;
        else color = RED;
        Player opponent(color);
        Player enemy = opponent;
        // end of construct
        
        for  (i = 0; i<ROW; i++)
        {
            for (j = 0; j<COL; j++)
            {
                copy_board = board;
                enemy = opponent;
                if (copy_board.place_orb(i, j, &enemy))
                {
                    eval = min_max_algorithm(copy_board, depth-1, alpha, beta, true, index, player);
                    if (eval < min) min = eval;
                    if (min < beta) beta = min;
                    if (alpha >= beta) break;
                }
            }
            if (j < COL) break;
        }
        return min;
    }
}

void algorithm_A(Board board, Player player, int index[]){

    // cout << board.get_capacity(0, 0) << endl;
    // cout << board.get_orbs_num(0, 0) << endl;
    // cout << board.get_cell_color(0, 0) << endl;
    // board.print_current_board(0, 0, 0);

    //////////// Random Algorithm ////////////
    // Here is the random algorithm for your reference, you can delete or comment it.
    index[0] = index[1] = INFINITY;
    int x = min_max_algorithm(board, MAX_DEPTH, -INFINITY, INFINITY, true, index, player);
    if (index[0] == INFINITY && index[1] == INFINITY)
        index[0] = index[1] = 0;
    
}