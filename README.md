# Game of Life simulator

## Summary and rules

This is a c implementation of John Conway's famous 'Game of Life'. The rules of the
'game' are simple. The universe of the game is a torus-connected two dimensional grid
filled with 'live' cells (denoted by 'X' in my implementation) and 'dead' cells (denoted
by ' '). At the beginning of the game, the grid is randomly populated with a some
distribution of live and dead cells, and cells are either born or destroyed based on
their interactions with their eight neighbors:

1. Any live cell with fewer than 2 living neighbors dies of loneliness.
2. Any live cell with more than 3 living neighbors dies of overcrowding.
3. A live cell with 2 or 3 neighbors continues on to the next generation
4. A dead cell surrounded by exactly 3 neighbors will be made live

Based on these rules, certain patterns occur, some dynamic, some static. For example,


    XX    and      XX
    XX            X  X
                   XX

are  static patterns. That is, they are stable. An example of a dynamic pattern is 
a 'blinker':

              X 
    XXX ----> X ----> XXX ----> ...
              X

and this movement, if uninhibited, will continue forever. There are patterns that move
around the board as well, but it is harder to illustrate their behavior through text.

## Implementation
I make use of the Pthreads library for this implementation. I am sure there are single
threaded versions of this game that are far simpler and more efficient than this version,
but the purpose of this project was to teach myself some basic threading techniques.
To that end, the code is way over-commented, and I also code my own pthread barrier, even
though the Pthread library includes a pre-made barrier.

## Bugs
There is a 'generation counter' that keeps track of the generation that we are on. 
Sometimes it is 1 or 2 generations off -- I'm not sure whether it's an issue with the 
generation counter display, or my simulation actually runs 1 or 2 generations less/more 
than desired. In any case, this is a relatively small error.
