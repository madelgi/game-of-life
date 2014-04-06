#include <stdio.h>
#include <stdlib.h>
#include "grid.h"
#include "tinfo.h"

// Allocates memory for a tinfo struct.
//
tinfo *initTinfo() {
  tinfo *T = (tinfo *)malloc(sizeof(tinfo));
  T->in = NULL;
  T->out = NULL;
  T->section = 0;
  T->divide = 0;
  return T;
}
