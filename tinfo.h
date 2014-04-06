#include "grid.h"

#ifndef _TINFO_H
#define _TINFO_H

// tinfo keeps track of the data we need to pass to
// each thread in GoL.c. It keeps track of two grids, one
// of which is our 'main' grid, and the other monitors
// evolved values. The tinfo struct also holds a few 
// integral values: gen holds the number of generations the
// GoL simulation will run, and section/divide are used
// to compute the section of the grid G that our thread will
// work on.
//
typedef struct {
  grid *in;
  grid *out;
  int section, divide;
  int gen;
} tinfo;

tinfo *initTinfo();

#endif
