#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/rules.h"
#include "../include/algorithm.h"

using namespace std;

#ifndef LEVEL
    #define LEVEL 4
#endif

#define WEST -1
#define EAST 1
#define NORTH 1
#define SOUTH -1

#define EASY_BOARD_EVALUATOR 0
#define HARD_BOARD_EVALUATOR 1

class TreeNode{
    private:
        Board boardState;
        TreeNode *nextNode;
        TreeNode *nextRoundNode;
        int index[2];    // Record which orb result in this TreeNode
    public:
        TreeNode();
        TreeNode(Board board);
        TreeNode(Board board, int x, int y);
        void setNextNode(TreeNode* TreeNode);
        void setNextRoundNode(TreeNode *TreeNode);
        TreeNode* getNextNode();
        TreeNode* getNextRoundNode();
        TreeNode* getTail();
        Board getBoardState();
        int getX();
        int getY();
};

class gameTree{
    private:
        TreeNode *root;
        Player *player;
        Player *opponent;
    public:
        gameTree(Board board, Player *player);
        TreeNode* chooseOrb(int difficulty);   // Use the four-level look-ahead game tree to determine the next step of orb
        int insertNextRound(TreeNode *Node, Player currentTurn);
        int levelLookAhead();  // Add a new level of look-ahead in gameTree
        int numAtLevel(TreeNode *Node); 
        float BoardEvaluator(TreeNode *Node, int difficulty);
};

/*-------------------------Implement class TreeNode functions---------------------------*/
TreeNode::TreeNode(Board board){
    this->boardState=board;
    nextNode = NULL;
    nextRoundNode = NULL;
    index[0]=0; index[1]=0;
}

TreeNode::TreeNode(Board board, int x, int y){
    this->boardState=board;
    nextNode = NULL;
    nextRoundNode = NULL;
    this->index[0]=x;
    this->index[1]=y;
}

void TreeNode::setNextNode(TreeNode *Node){
    this->nextNode=Node;
}

void TreeNode::setNextRoundNode(TreeNode *Node){
    this->nextRoundNode=Node;
}

TreeNode* TreeNode::getNextNode(){
    return this->nextNode;
}

TreeNode* TreeNode::getNextRoundNode(){
    return this->nextRoundNode;
}

TreeNode* TreeNode::getTail(){
    TreeNode* temp=NULL;
    TreeNode* tempNext=this;
    while(tempNext!=NULL){
        temp = tempNext;
        tempNext = tempNext->getNextNode();
    }
    return temp;
}

Board TreeNode::getBoardState(){
    return this->boardState;
}

int TreeNode::getX(){
    return this->index[0];
}

int TreeNode::getY(){
    return this->index[1];
}

/*------------------------Implement class gameTree functions----------------------------*/
gameTree::gameTree(Board board, Player *player){
    this->root = new TreeNode(board);
    this->player = player;
    char colorPlayer = player->get_color();
    char colorOpponent = 'v'; // virtual player
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            if(board.get_cell_color(i,j)!='w' && board.get_cell_color(i,j)!=colorPlayer)
                colorOpponent = board.get_cell_color(i,j);
        }
    }
    this->opponent = new Player(colorOpponent);
}

TreeNode* gameTree::chooseOrb(int difficulty){
    this->insertNextRound(this->root, *this->player);
    TreeNode *firstLevelNode = this->root->getNextRoundNode();
    TreeNode *bestNode = this->root->getNextRoundNode();
    float bestScore = 0;
    float tempScore = 0;
    long numRoundNode = 0;
    
    for(firstLevelNode;firstLevelNode==NULL;firstLevelNode=firstLevelNode->getNextNode()){
        // Find out which one is the best to put the orb
        tempScore = this->BoardEvaluator(firstLevelNode, difficulty);
        // Second level    
        numRoundNode = insertNextRound(firstLevelNode, *this->opponent);
        TreeNode *secondLevelNode = firstLevelNode->getNextRoundNode();
        for(secondLevelNode;secondLevelNode==NULL;secondLevelNode=secondLevelNode->getNextNode()){
            tempScore += 0.8*this->BoardEvaluator(secondLevelNode, difficulty)/numRoundNode;
            numRoundNode = numRoundNode*insertNextRound(secondLevelNode, *this->player);
            TreeNode *thirdLevelNode = secondLevelNode->getNextRoundNode();
            
            for(thirdLevelNode;thirdLevelNode==NULL;thirdLevelNode=thirdLevelNode->getNextNode()){
                tempScore += 0.6*this->BoardEvaluator(thirdLevelNode, difficulty)/numRoundNode;
                numRoundNode = numRoundNode*insertNextRound(thirdLevelNode, *this->opponent);
                TreeNode *fourthLevelNode = thirdLevelNode->getNextRoundNode();
                for(fourthLevelNode;fourthLevelNode==NULL;fourthLevelNode=fourthLevelNode->getNextNode()){
                    tempScore += 0.4*this->BoardEvaluator(fourthLevelNode, difficulty)/numRoundNode;
                }
            }
            
        }
        if(tempScore>bestScore){
            bestScore = tempScore;
            bestNode = firstLevelNode;
        }
    }
    return bestNode;
}

int gameTree::insertNextRound(TreeNode *Node, Player currentTurn){
    // Insert all the cases of next round in gameTree
    // Input : currentTurn indicates which player is going to put the orb in this turn
    int numNewState=0;
    char color = currentTurn.get_color();
    int headX, headY;
    bool headFind=false;
    Board board = Node->getBoardState();
    
    // Find first next round node using random
    while(1){
        headX = rand()%5;
        headY = rand()%6;
        if(board.get_cell_color(headX,headY)==color || board.get_cell_color(headX,headY)=='w'){
            numNewState++;
            board.place_orb(headX,headY,&currentTurn);
            Board newBoardState = board;
            TreeNode *newNode = new TreeNode(newBoardState,headX,headY);
            Node->setNextRoundNode(newNode);
            break;
        }
    }
    
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            if(board.get_cell_color(i,j)==color || board.get_cell_color(i,j)=='w'){
                numNewState++;
                board.place_orb(i,j,&currentTurn);
                Board newBoardState = board;
                TreeNode *newNode = new TreeNode(newBoardState,i,j);
                TreeNode *tail = Node->getNextRoundNode()->getTail();
                tail->setNextNode(newNode);
                
            }
        }
    }  
    return numNewState;
}

float gameTree::BoardEvaluator(TreeNode *Node, int difficulty){
    // This is the simple board evaluator
    Board board = Node->getBoardState();
    char colorPlayer = player->get_color();
    char colorOpponent = opponent->get_color();
    float orbPlayer = 0, orbOpponent = 0;
    float orbScore = 0;
    int orbX = Node->getX();
    int orbY = Node->getY();
    if(difficulty == EASY_BOARD_EVALUATOR){
        for(int i=0;i<ROW;i++){
            for(int j=0;i<COL;j++){
                // 只考慮座標(i,j)的orb數量
                if(board.get_cell_color(i,j)==colorPlayer){
                    orbPlayer+=board.get_orbs_num(i,j);
                }else if(board.get_cell_color(i,j)==colorOpponent){
                    orbOpponent+=board.get_orbs_num(i,j);
                }
            }
        }
        orbScore = orbPlayer - orbOpponent;
    }else if(difficulty == HARD_BOARD_EVALUATOR){
        for(int i=0;i<ROW;i++){
            for(int j=0;j<COL;j++){
                // 考慮座標(i,j)的orb數量以及四個方向的對手orb數量
                if(board.get_cell_color(i,j)==colorPlayer){
                    orbPlayer+=board.get_orbs_num(i,j);
                }else if(board.get_cell_color(i,j)==colorOpponent){
                    orbOpponent+=board.get_orbs_num(i,j);
                }
            }
        }
        // 計算四個方向的opponent orb數量(越多代表這個cell越值得引爆)
        if(board.get_cell_color(orbX,orbY)==colorPlayer){
            if(index_range_illegal(orbX+EAST,orbY)==true){
                if(board.get_cell_color(orbX+EAST,orbY)==colorOpponent){
                    if(board.get_orbs_num(orbX+EAST,orbY)<board.get_orbs_num(orbX,orbY))
                    {
                        orbScore += (board.get_orbs_num(orbX+EAST,orbY));
                    }  
                    else
                    {
                        orbScore -= (board.get_orbs_num(orbX,orbY));
                    } 
                }     
            }
            if(index_range_illegal(orbX-WEST,orbY)==true){
                if(board.get_cell_color(orbX-WEST,orbY)==colorOpponent){
                    if(board.get_orbs_num(orbX-WEST,orbY)<board.get_orbs_num(orbX,orbY))
                    {
                        orbScore += (board.get_orbs_num(orbX-WEST,orbY));
                    }  
                    else
                    {
                        orbScore -= (board.get_orbs_num(orbX,orbY));
                    } 
                }
            }
            if(index_range_illegal(orbX,orbY+NORTH)==true){
                if(board.get_cell_color(orbX,orbY+NORTH)==colorOpponent){
                    if(board.get_orbs_num(orbX,orbY+NORTH)<board.get_orbs_num(orbX,orbY))
                    {
                        orbScore += (board.get_orbs_num(orbX,orbY+NORTH));
                    }  
                    else
                    {
                        orbScore -= (board.get_orbs_num(orbX,orbY));
                    } 
                }
            }
            if(index_range_illegal(orbX,orbY-SOUTH)==true){
                if(board.get_cell_color(orbX,orbY-SOUTH)==colorOpponent){
                    if(board.get_orbs_num(orbX,orbY-SOUTH)<board.get_orbs_num(orbX,orbY))
                    {
                        orbScore += (board.get_orbs_num(orbX,orbY-SOUTH));
                    }  
                    else
                    {
                        orbScore -= (board.get_orbs_num(orbX,orbY));
                    } 
                }
            }
        } 
        orbScore += (orbPlayer-orbOpponent);
    }
    return orbScore;
}


void algorithm_B(Board board, Player player, int index[]){
    // algorithm_B is randomMove version bot
    srand(time(NULL)*time(NULL));
    int row, col;
    char color = player.get_color();
    
    while(1){
        row = rand() % 5;
        col = rand() % 6;
        if(board.get_cell_color(row, col) == color || board.get_cell_color(row, col) == 'w') break;
    }

    index[0] = row;
    index[1] = col;
}

void algorithm_C(Board board, Player player, int index[]){
    // algorithm_C is noLook version bot
    int row, col;
    char color = player.get_color();

    // Scan through the board to find a cell that has min (capacity-orbNum)
    int orbLack;
    while(1){
        row = rand() % 5;
        col = rand() % 6;
        if(board.get_cell_color(row, col) == color || board.get_cell_color(row, col) == 'w') break;
    }
    orbLack = board.get_capacity(row,col)-board.get_orbs_num(row,col);

    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            if(board.get_cell_color(i,j)==color){
                if( (board.get_capacity(i,j)-board.get_orbs_num(i,j))<=orbLack ){
                    row=i;
                    col=j;
                }
            }
        }
    }
    index[0]=row;
    index[1]=col;
}

void algorithm_D(Board board, Player player, int index[]){
    // algorithm_D is heithoff version bot
    gameTree gt(board,&player);
    TreeNode *orbNode = gt.chooseOrb(EASY_BOARD_EVALUATOR);
    index[0] = orbNode->getX();
    index[1] = orbNode->getY();
}

void algorithm_E(Board board, Player player, int index[]){
    // algorithm_E is heithoff version bot
    gameTree gt(board,&player);
    TreeNode *orbNode = gt.chooseOrb(HARD_BOARD_EVALUATOR);
    index[0] = orbNode->getX();
    index[1] = orbNode->getY();
}


//// testing ////
/*
# define INFINITY 9999999
# define MAX_DEPTH 5

int my_evaluate2(Board board, Player player)
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
    
    int i, j, adjROW, adjCOL, current_num, current_capacity, adj_num, adj_capacity;
    bool IsGoodCell;
    int point = 0;
    
    for (i = 0; i<ROW; i++)
    {
        for (j = 0; j<COL; j++)
        {
            // check the adj cell of my cell
            if (board.get_cell_color(i, j) == my_color)
            {                
                IsGoodCell = true;
                current_num = board.get_orbs_num(i, j);
                current_capacity = board.get_capacity(i, j);
                point += current_num;

                adjROW = i+1;
                adjCOL = j;
                if (adjROW < ROW)
                {
                    adj_num = board.get_orbs_num(adjROW, adjCOL);
                    adj_capacity = board.get_capacity(adjROW, adjCOL);
                    if (adj_capacity - adj_num == 1 && board.get_cell_color(adjROW, adjCOL) == enemy_color)
                    {
                        point -= 5 - current_capacity;
                        IsGoodCell = false;
                    }
                }

                adjROW = i-1;
                adjCOL = j;
                if (adjROW >= 0)
                {
                    adj_num = board.get_orbs_num(adjROW, adjCOL);
                    adj_capacity = board.get_capacity(adjROW, adjCOL);
                    if (adj_capacity - adj_num == 1 && board.get_cell_color(adjROW, adjCOL) == enemy_color)
                    {
                        point -= 5 - current_capacity;
                        IsGoodCell = false;
                    }
                }

                adjROW = i;
                adjCOL = j+1;
                if (adjCOL < COL)
                {
                    adj_num = board.get_orbs_num(adjROW, adjCOL);
                    adj_capacity = board.get_capacity(adjROW, adjCOL);
                    if (adj_capacity - adj_num == 1 && board.get_cell_color(adjROW, adjCOL) == enemy_color)
                    {
                        point -= 5 - current_capacity;
                        IsGoodCell = false;
                    }
                }

                adjROW = i;
                adjCOL = j-1;
                if (adjCOL >= 0)
                {
                    adj_num = board.get_orbs_num(adjROW, adjCOL);
                    adj_capacity = board.get_capacity(adjROW, adjCOL);
                    if (adj_capacity - adj_num == 1 && board.get_cell_color(adjROW, adjCOL) == enemy_color)
                    {
                        point -= 5 - current_capacity;
                        IsGoodCell = false;
                    }
                }

                // some weighting
                if (IsGoodCell)
                {
                    if (current_capacity == 2)
                        point += 3;
                    else if (current_capacity == 3)
                        point += 2;
                    else{}

                    if (current_capacity - current_num == 1)
                        point += 2;
                    else{}
                }
            }
            // end of check
        }
    }

    return point;
}

int min_max_algorithm2(Board board, int depth, int alpha, int beta, bool IsMaxLevel, int index[], Player player)
{
    Board copy_board;

    if (depth == 0)
    {
        Player me = player;
        copy_board = board;
        return my_evaluate2(copy_board, me);
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
                    if (copy_board.win_the_game(me)) eval = INFINITY;
                    else eval = min_max_algorithm2(copy_board, depth-1, alpha, beta, false, index, player);

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
                    if (copy_board.win_the_game(enemy)) eval = -INFINITY;
                    else eval = min_max_algorithm2(copy_board, depth-1, alpha, beta, true, index, player);
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

void algorithm_B(Board board, Player player, int index[]){

    index[0] = index[1] = INFINITY;
    int x = min_max_algorithm2(board, MAX_DEPTH, -INFINITY, INFINITY, true, index, player);
    if (index[0] == INFINITY && index[1] == INFINITY)
    {
        // randomly select a point
        int i,j;
        char color = player.get_color();
        for (i = 0; i<ROW; i++)
        {
            for (j = 0; j<COL; j++)
            {
                if(board.get_cell_color(i, j) == color || board.get_cell_color(i, j) == 'w') break;
            }
        }
        index[0] = i;
        index[1] = j;
    }
}
*/