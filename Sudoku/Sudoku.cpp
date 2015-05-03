// Sudoku.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdlib.h>
#include <queue>
#include "Node.h"
#include <ctime>
#include <stdio.h>
#include <iostream>

#define SUDOKU_SIZE		9
#define CELL_LOCKED		1
#define CELL_UNLOCKED	0
#define LOCK_INDEX		0
#define COUNT_INDEX		10

typedef struct quad
{
	int quad_i;
	int quad_j;
};

//Prints a sudoku board
void print_sudoku(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE]);

//Gets the quadrant of a sudoku index
quad get_quad(int sud_index_i, int sud_index_j)
{
	quad ret;
	ret.quad_i = sud_index_i/3;
	ret.quad_j = sud_index_j/3;

	return ret;
}

//Checks if putting num in (i_index,j_index) is a valid move
bool check_valid_sudoku_move(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE], int i_index, int j_index, int num)
{
	quad ind_quad = get_quad(i_index,j_index);

	int i_start = ind_quad.quad_i*3;
	int i_end = i_start+3;

	int j_start = ind_quad.quad_j*3;
	int j_end = j_start+3;

	//Check if number exists horizontally or vertically
	for(int i = 0;i<SUDOKU_SIZE;i++)
	{
		if(i != i_index)
		{
			if(sudoku[i][j_index] == num)
				return false;

		}

		if(i != j_index)
		{
			if(sudoku[i_index][i] == num)
				return false;
		}
	}

	//Check if number exists in the quadrant
	for(int i = i_start;i<i_end;i++)
	{
		for(int j = j_start;j<j_end;j++)
		{
			if((i!=i_index || j!=j_index)  && (sudoku[i][j] == num))
				return false;
		}
	}

	return true;
}

//Checks whether a cell has no possible moves past cell (from_i,from_j)
bool check_deadlock_situation(int sudoku_config[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE+2], int from_i, int from_j)
{

	for(int j = from_j;j<SUDOKU_SIZE;j++)
		if(sudoku_config[from_i][j][LOCK_INDEX] != CELL_LOCKED && sudoku_config[from_i][j][COUNT_INDEX] == 0)
			return true;

	from_i++;

	if(from_i >= SUDOKU_SIZE)
		return false;

	for(int i = from_i;i<SUDOKU_SIZE;i++)
	{
		for(int j = 0;j<SUDOKU_SIZE;j++)
		{
			if(sudoku_config[i][j][LOCK_INDEX] != CELL_LOCKED && sudoku_config[i][j][COUNT_INDEX] == 0)
				return true;
		}
	}

	return false;
}

//Initialize the sudoku possibilities config
void init_sudoku_config(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE], int sudoku_config[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE+2])
{
	for(int i = 0;i<SUDOKU_SIZE;i++)
	{
		for(int j = 0;j<SUDOKU_SIZE;j++)
		{
			sudoku_config[i][j][COUNT_INDEX] = 0;
			if(sudoku[i][j] != 0)
			{
				sudoku_config[i][j][LOCK_INDEX] = CELL_LOCKED;
				continue;
			}
			else
			{
				sudoku_config[i][j][LOCK_INDEX] = CELL_UNLOCKED;
				
				for(int k = 1;k < SUDOKU_SIZE+1;k++)
				{
					bool valid_num = check_valid_sudoku_move(sudoku,i,j,k);
					sudoku_config[i][j][k] = valid_num;
					sudoku_config[i][j][COUNT_INDEX] += valid_num;
				}
			}
		}
	}
}

//Prints a sudoku number of possibilities for each cell
void print_sudoku_config(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE+2])
{
	int i_print_ind = 0;
	int j_print_ind = 0;
	printf("\n");
	for(int i = 0;i < SUDOKU_SIZE + 2;i++)
	{
		for(int j = 0;j < SUDOKU_SIZE + 2;j++)
		{
			if( i == 3 || i == 7 )
			{
				printf(" - ");
			}
			else if( j == 3 || j == 7 )
			{
				printf(" | ");
			}
			else
			{
				if(i > 7)
				{
					i_print_ind = i - 2;
				}
				else if(i > 3)
				{
					i_print_ind = i - 1;
				}
				else
				{
					i_print_ind = i;
				}

				if(j > 7)
				{
					j_print_ind = j - 2;
				}
				else if(j > 3)
				{
					j_print_ind = j - 1;
				}
				else
				{
					j_print_ind = j;
				}

				printf(" %d ", sudoku[i_print_ind][j_print_ind][COUNT_INDEX]);
			}

		}
		printf("\n");


	}
}

//Get a new number for the cell (i_new,j_new) that is valid based on history
int generate_number(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE], int i_new, int j_new, Node* node)
{
	for(int i = 1;i<SUDOKU_SIZE+1;i++)
	{
		if(check_valid_sudoku_move(sudoku,i_new,j_new,i) && node->explored_nums[i - 1] == 0)
			return i;
	}

	return -1;
}

//advances the cursor one cell to the right in the sudoku board
void advance_cursor(int *i_cur, int *j_cur)
{
	(*j_cur)++;
	if(*j_cur == SUDOKU_SIZE)
	{
		(*j_cur) = 0;
		(*i_cur)++;
	}
}

//retracts the cursor one cell to the left in the sudoku board
void retract_cursor(int* i, int* j)
{
	(*j)--;
	if(*j < 0)
	{
		*j = SUDOKU_SIZE-1;
		(*i)--;
	}
}

//Tries to generate a valid number for the cell (i,j) and puts it in the sudoku board
bool make_move(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE], int sudoku_config[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE+2], int i, int j,Node * node)
{
	int num = generate_number(sudoku,i,j,node);
	if(num == -1)
		return false;
	sudoku[i][j] = num;


	node->explored_nums[num-1] = 1;
	Node new_node;
	node->children[num-1] = &new_node;
	*(new_node.parent) = *node;
	*node = new_node;

	return true;
}

//removes the last number added in the sudoku board
void undo_move(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE], int sudoku_config[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE+2], int* i, int* j)
{
	
	retract_cursor(i,j);

	if(*i < 0)
		return;

	while(sudoku_config[*i][*j][LOCK_INDEX])
	{
		retract_cursor(i,j);

		if(*i < 0)
			return;
	}


	sudoku[*i][*j] = 0;

}

//solves a given sudoku board using DFS
void solve_sudoku(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE])
{
	int sudoku_config[SUDOKU_SIZE][SUDOKU_SIZE][SUDOKU_SIZE+2];
	
	init_sudoku_config(sudoku,sudoku_config);

	bool solved = false;
	bool deadlock = false;
	int cur_i = 0;
	int cur_j = 0;
	int new_num = 0;

	Node new_node;
	Node *curr_node = &new_node;
	
	
	
	while(!solved)
	{
		//Ignore a locked cell
		if(sudoku_config[cur_i][cur_j][LOCK_INDEX] == CELL_LOCKED)
		{
			cur_j++;
			if(cur_j == SUDOKU_SIZE)
			{
				cur_j = 0;
				cur_i++;
			}

			if(cur_i >= SUDOKU_SIZE)
				solved = true;

			continue;
		}


		bool move_made = make_move(sudoku,sudoku_config,cur_i,cur_j,curr_node);

		if(!move_made)
		{
			undo_move(sudoku,sudoku_config,&cur_i,&cur_j);
			curr_node = curr_node->parent;
			continue;
		}


		advance_cursor(&cur_i,&cur_j);

		if(cur_i >= SUDOKU_SIZE)
		{
			solved = true;
			continue;
		}


	}
}

//a hard sudoku puzzle
void fill_sudoku_hard(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE])
{
	sudoku[0][0] = 8;

	sudoku[1][2] = 3;
	sudoku[1][3] = 6;

	sudoku[2][1] = 7;
	sudoku[2][4] = 9;
	sudoku[2][6] = 2;

	sudoku[3][1] = 5;
	sudoku[3][5] = 7;

	sudoku[4][4] = 4;
	sudoku[4][5] = 5;
	sudoku[4][6] = 7;

	sudoku[5][3] = 1;
	sudoku[5][7] = 3;

	sudoku[6][2] = 1;
	sudoku[6][7] = 6;
	sudoku[6][8] = 8;

	sudoku[7][2] = 8;
	sudoku[7][3] = 5;
	sudoku[7][7] = 1;

	sudoku[8][1] = 9;
	sudoku[8][6] = 4;
}

//an easy sudoku puzzle
void fill_sudoku(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE])
{
	sudoku[0][2] = 7;
	sudoku[0][7] = 2;
	sudoku[1][5] = 1;
	sudoku[1][8] = 7;
	sudoku[4][1] = 9;
	sudoku[6][2] = 6;
	sudoku[6][7] = 7;
	sudoku[7][3] = 1;
	sudoku[8][1] = 3;
	sudoku[8][7] = 6;
}

int main(int argc, _TCHAR* argv[])
{

	int sudoku[SUDOKU_SIZE][SUDOKU_SIZE];

	

	for(int i = 0;i<SUDOKU_SIZE;i++)
	{
		for(int j = 0;j<SUDOKU_SIZE;j++)
		{
			sudoku[i][j] = 0;
		}
	}

	fill_sudoku_hard(sudoku);

	print_sudoku(sudoku);

	std::clock_t start;
    double duration;

    start = std::clock();

	solve_sudoku(sudoku);

	print_sudoku(sudoku);

	duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

    printf("Duration: %f\n",duration);

	getchar();
	return 0;
}

//prints a sudoku board to the console
void print_sudoku(int sudoku[SUDOKU_SIZE][SUDOKU_SIZE])
{
	int i_print_ind = 0;
	int j_print_ind = 0;
	printf("\n");
	for(int i = 0;i < SUDOKU_SIZE + 2;i++)
	{
		for(int j = 0;j < SUDOKU_SIZE + 2;j++)
		{
			if( i == 3 || i == 7 )
			{
				printf(" - ");
			}
			else if( j == 3 || j == 7 )
			{
				printf(" | ");
			}
			else
			{
				if(i > 7)
				{
					i_print_ind = i - 2;
				}
				else if(i > 3)
				{
					i_print_ind = i - 1;
				}
				else
				{
					i_print_ind = i;
				}

				if(j > 7)
				{
					j_print_ind = j - 2;
				}
				else if(j > 3)
				{
					j_print_ind = j - 1;
				}
				else
				{
					j_print_ind = j;
				}

				if(sudoku[i_print_ind][j_print_ind] == 0)
				printf(" . ");
				else
				printf(" %d ", sudoku[i_print_ind][j_print_ind]);
			}

		}
		printf("\n");


	}
}
