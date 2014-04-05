#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grid.h"

// Allocates memory for a grid (matrix) of dimensions
// rows x cols. 
//
grid *initGrid(int rows, int cols) {
	int i;
	grid *G = (grid *)malloc(sizeof(grid));	
	G->rows = rows;
	G->cols = cols;
	G->val = malloc(rows*sizeof(int *));
	for (i=0; i<rows; i++) {
		G->val[i] = malloc(cols*sizeof(int));
	}
	return G;
}

// 
// populate
//
// This function randomly populates our grid with
// 1's and 0's.
//
void populate(grid *G) {
	srand((unsigned int) time(NULL));

	int i, j, k;
	for (i=0; i<G->rows; i++) {
		for (j=0; j<G->cols; j++) {

			// To randomly populate the grid, we randomly compute
			// k, which will be either 0, 1, or 2. If k=0, we place
			// a 1 in G->val[i][j]. Otherwise, we place a 0. Theoretically,
			// our grid should be 1/3 1's and 2/3 0's.
			//
			k = rand() % 3;
			if (k == 0) G->val[i][j] = 1;
			else G->val[i][j] = 0;
		}
	}

}
