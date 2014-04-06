#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "grid.h"
#include "tinfo.h"
#include "mybarrier.h"

// Initiate a barrier object
//
mybarrier barr;

// We use this global variable
// to keep track of when a grid should be
// printed. 
//
int count = 0;

// Keeps track of which generation we are
// on
int generation=0;

// countNeighbors
//
// This function counts the neighbors of a point in our grid. It is relatively
// simple, but the fact that our board is a torus makes things a bit more
// difficult. The code is divided into three main parts: center cases, edge
// cases, and corner cases.
//
int countNeighbors(grid *G, int x, int y) {
  int n=0, i, j;
  
  // Center case
  //
  if (x>0 && y>0 && x<G->rows-1 && y<G->cols-1) {
    for (j=y-1; j<y+2; j++) {
      for (i=x-1; i<x+2; i++) {
        if (i == x && j == y) {
          continue;
        }
        else {
          n += G->val[i][j];
        }
      }
    }
  } 
  
  // The following four cases are border cases, excluding
  // corner points. The cases afterwords are the four
  // corner cases. What follows isn't necessarily pretty,
  // and I'm sure there are better ways to do this, but
  // I couldn't come up with anything that elegant.
  //
  else if (y==0 && x>0 && x<G->rows-1) {
    for (j=y; j<y+2; j++) {
      for (i=x-1; i<x+2; i++) {
        if (i == x && j == y) continue;
          else n += G->val[i][j];
      }
    }
    
    j = G->cols-1;
    for (i=x-1; i<x+2; i++) {
      n += G->val[i][j];
    } 

  } else if (y==G->cols-1 && x>0 && x<G->rows-1) {
    for (j=y-1; j<y+1; j++) {
      for (i=x-1; i<x+2; i++) {
        if (i == x && j == y) continue;
        else n+= G->val[i][j];
      }
    }

    j=0;
    for (i=x-1; i<x+2; i++) {
      n += G->val[i][j];
    }
  } else if (x==0 && y>0 && y<G->cols-1) {
    for (j=y-1; j<y+2; j++) {
      for (i=x; i<x+2; i++) {
        if (i==x && j==y) continue;
        else n += G->val[i][j];
      }
    }

    i = G->rows-1;
    for (j=y-1; j<y+2; j++) {
      n += G->val[i][j];
    }
  } else if (x==G->rows-1 && y>0 && y<G->cols-1) {
    for (j=y-1; j<y+2; j++) {
      for (i=x-1; i<x+1; i++) {
        if (i==x && j==y) continue;
        else n += G->val[i][j];
      }
    }

    i = 0;
    for (j=y-1; j<y+2; j++) {
      n += G->val[i][j];
    }
  }

  // Finally, we deal with the four corner
  // cases. Again, this is more of the same as
  // above. A lot of silly fussing around with 
  // indices. The four cases, in order, are:
  //  (1) Top left
  //  (2) Top right
  //  (3) Bottom left
  //  (4) Bottom right
  //
  else if (x==0 && y==0) {
    for (i=0; i<2; i++) {
      for (j=0; j<2; j++) {
        if (i==0 && j==0) continue;
        else n += G->val[i][j];
      }
    }

    i=G->rows-1;
    for (j=0; j<2; j++) {
      n += G->val[i][j];
    }

    j=G->cols-1;
    for (i=0; i<2; i++) {
      n +=G->val[i][j];
    }
    n += G->val[G->rows-1][G->cols-1];

  } else if (x==0 && y==G->cols-1) {
    for (i=0; i<2; i++) {
      for (j=G->cols-2; j<G->cols; j++) {
        if (i==0 && j==G->cols-1) continue;
        else n += G->val[i][j];
      }
    }

    j=0;
    for (i=0; i<2; i++) {
      n += G->val[i][j];
    }

    i=G->rows-1;
    for (j=G->cols-2; j<G->cols; j++) {
      n += G->val[i][j];
    }
    n += G->val[G->rows-1][0];

  } else if (x==G->rows-1 && y==0) {
    for (i=G->rows-2; i<G->rows; i++) {
      for (j=0; j<2; j++) {
        if (i==G->rows-1 && j==0) continue;
        else n += G->val[i][j];
      }
    }

    i=0;
    for (j=0; j<2; j++) {
      n += G->val[i][j];
    }

    j=G->cols-1;
    for (i=G->rows-2; i<G->rows; i++) {
      n += G->val[i][j];
    }
    n += G->val[0][G->cols-1];

  } else {
    for (i=G->rows-2; i<G->rows; i++) {
      for (j=G->cols-2; j<G->cols; j++) {
        if (i==G->rows-1 && j==G->cols-1) continue;
        else n+= G->val[i][j];
      }
    }
    
    i=0;
    for (j=G->cols-2; j<G->cols; j++) {
      n += G->val[i][j];
    }

    j=0;
    for (i=G->rows-2; i<G->rows; i++) {
      n += G->val[i][j];
    }
    n += G->val[0][0];  
  } 

  return n;


}

//
// mEvolve
//
// mEvolve looks at a section of the grid G (determined by the thread), and
// proceeds to count the neighbors for each entry. Based on the neighbor
// count, the function passes a value indicating cell death (0) to T, or
// cell birth (1) to T. 
//
void *mEvolve(grid *G, grid *T, int height, int part) {
  
  int i, j, neighbors;

  // Examine a specific part of G
  //
  for (i=part; i<(part+height); i++) {
    for (j=0; j<G->cols; j++) {

      neighbors = countNeighbors(G, i, j);
      
      // Determine which cells are born and which die.
      //
      if (G->val[i][j] == 1 && (neighbors < 2 || neighbors > 3)) {
        T->val[i][j] = 0;
      } else if (G->val[i][j] == 0 && neighbors == 3) {
        T->val[i][j] = 1;
      }
    }
  }   
}

//
// mgridUpdate
//
// Looks at a specific part of our temp grid, T, and transfers
// the values into our permanent grid, G. The values transfered
// depend on the thread that calls the function.
//
void *mgridUpdate(grid *G, grid *T, int height, int part) {
  
  int i, j;
  for (i=part; i<(part + height); i++) {
    for (j = 0; j<G->cols; j++) {
      G->val[i][j] = T->val[i][j];
    }
  }
}

// 
// mFunc
//
// mFunc is the general function passed to each thread. It is responsible
// for computing the evolved values of a certain section of G (mEvolve),
// and then updating G to contain these values (mgridUpdate).
//
void *mFunc(void *arguments) {
  tinfo *I = (tinfo *)arguments;

  grid *G = I->in;
  grid *T = I->out;
  int rows = G->rows;
  int cols = G->cols;
  int sect = I->section;
  int div = I->divide;
  int i=0;

  int height = (int)(rows/div);
  int part = height*sect;
  
  // multEvolve looks at the entries in G and, based on those
  // entries, enters new values into T. Therefore, we can have
  // simultaneous threads accessing multEvolve with no problem 
  // (assuming we have allocated independent parts of G for
  // each thread to work on). We place a barrier before and after
  // running mgridUpdate, as mgridUpdate edits the values in G.
  // We need each thread to be working with the same G when multEvolve
  // runs. 
  //
  while (i<I->gen) {
    mEvolve(G, T, height, part);
    barrier_wait(&barr);
    mgridUpdate(G, T, height, part);
    count++;
    
    // This is the only relatively tricky part of the loop. 
    // The last thread to complete the above computations
    // should [theoretically] satisfy count % div == 0, and
    // then print the grid. If we didn't have the 'if' condition,
    // our grid would print 'div' times for each generation, which 
    // is undesirable.
    //
    if (count % div == 0) {
      sleep(2000);
      printGrid(G);
      generation++;
    }
    barrier_wait(&barr);
    i++;
  }
}

//
// sleep
//
// We use sleep before printing the grid in the above loop. By 
// making the program wait for a specified amount of time before 
// printing, we get a clearer evolution process. Without the 
// 'sleep(2000)' above, the program prints each evolved state too quickly.
//
void sleep(unsigned int mill) {
  clock_t start = clock();
  while (clock() - start < mill) { }
}

// 
// printGrid
//
// Prints the grid -- relatively straightforward implementation.
// Replaces the 1's and 0's within the grid by X's and blank
// spaces, respectively.
//
void printGrid(grid *G) {
  system("clear");
  int i, j;
  
  printf("Welcome to the game of life! Generation: %d.\n", generation);
  for (i=0; i<G->rows; i++) {
    for (j=0; j<G->cols; j++) {
      switch (G->val[i][j]) {
        case 0: putchar(' '); break;
        case 1: putchar('X'); break;
        default: break;
      }
    }
    putchar('\n');
  }

}

// 
// main
//
// Run a game of life simulation.
//
int main() {
  int i, g, rows, cols;
  int div;
  
  // The first several lines take input parameters
  // for the game.
  //
  printf("Welcome to the Game of Life.\n");
  printf("How many generations would you like to watch? ");
  scanf("%d", &g);
  printf("Enter the width of the board: ");
  scanf("%d", &cols);
  printf("Enter the height of the board: ");
  scanf("%d", &rows);
  
  // Define our grids: G is our main grid, and T is our
  // temp grid. We also print the initial state of the grid
  // before actually running the simulation.
  //
  grid *G = initGrid(rows, cols);
  grid *T = initGrid(rows, cols);
  populate(G);
  printGrid(G);
  mgridUpdate(T, G, G->rows, 0);  

  // Gets the desired number of threads from the user -- we repeatedly
  // ask for a number until we get a divisor of rows. Once we know how
  // many threads there will be, we initialize the barrier.
  //
  printf("Please enter a divisor of %d to determine the number of threads: ", rows);
  scanf("%d", &div);
  while (rows % div != 0) {
    printf("I'm sorry, %d does not divide %d. Please choose a divisor of %d: ", div, rows, rows);
    scanf("%d", &div);
  }

  barrier_init(&barr, div);
  
  // Creates an array of tinfo structs and
  // pthreads. We then place the necessary 
  // info into each tinfo struct.
  //
  tinfo **I = malloc(div*sizeof(tinfo));
  pthread_t threads[div];
  
  for (i=0; i<div; i++) {
    I[i] = initTinfo();
    I[i]->in = G;
    I[i]->out = T;
    I[i]->section = i;
    I[i]->divide = div;
    I[i]->gen = g;
  }
  
  // Initialize a number of threads. Each thread works on a portion of our
  // grid -- which portion it works on is decided by the I[i] tinfo struct.
  //  
  for (i=0; i<div; i++) {
    pthread_create(&threads[i], NULL, &mFunc, (void *)I[i]);
  }

  // My implementation requires join, because the main thread
  // must wait for all of the child threads to complete before
  // destroying the barrier and printing the final grid.
  //
  for (i=0; i<div; i++) {
    pthread_join(threads[i], NULL);
  }
  
  // Destroy the barrier, print the final generation.
  //
  barrier_destroy(&barr);
  printGrid(G);

  return 0;
}
