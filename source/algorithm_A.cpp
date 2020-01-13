#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"

using namespace std;
# define INFINITY 9999999
# define MAX_DEPTH 5

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

int my_evaluate(Board board, int row, int column, PLAYER player){}

int min_max_algorithm(Board board, int depth, int alpha, int beta, bool IsMaxLevel, int index[], Player player)
{
    Board copy_board;

    if (depth == 0)
    {
        copy_board = board;
        return my_evaluate(copy_board, index[0], index[1], player);
    }

    int i, j, eval;
    if (IsMaxLevel)
    {
        int max = -INFINITY;
        Player me = player;
        for (i = 0; i<5; i++)
        {
            for (j = 0; j<6; j++)
            {
                copy_board = board;
                if (copy_board.place_orb(i, j, &me))
                {
                    eval = min_max_algorithm(copy_board, depth-1, alpha, beta, false, index, player);
                    if (eval > alpha) 
                    {
                        alpha = eval;
                        // update
                        if (depth == MAX_DEPTH)
                        {
                            index[0] = i;
                            index[1] = j;
                        }
                    }
                    if (alpha >= beta) break;
                }
            }
            if (j < 6) break;
        }

        if (alpha == -INFINITY && depth == MAX_DEPTH)
        {
            index[0] = 0;
            index[1] = 0;
        }

        return alpha;
    }

    else
    {
        // construct opponent
        char color = player.get_color();
        if (color == RED) color = BLUE;
        else color = RED;
        Player opponent(color);
        // end of construct
        
        for  (i = 0; i<5; i++)
        {
            for (j = 0; j<6; j++)
            {
                copy_board = board;
                if (copy_board.place_orb(i, j, &opponent))
                {
                    eval = min_max_algorithm(copy_board, depth-1, alpha, beta, true, index, player);
                    if (beta > eval)
                    {
                        beta = eval;
                        // Note we don't need to update index
                    }
                    if (alpha >= beta) break;
                }
            }
            if (j < 6) break;
        }
        return beta;
    }
    
}

void algorithm_A(Board board, Player player, int index[]){

    // cout << board.get_capacity(0, 0) << endl;
    // cout << board.get_orbs_num(0, 0) << endl;
    // cout << board.get_cell_color(0, 0) << endl;
    // board.print_current_board(0, 0, 0);

    //////////// Random Algorithm ////////////
    // Here is the random algorithm for your reference, you can delete or comment it.
    
}