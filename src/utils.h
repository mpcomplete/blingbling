#ifndef _UTILS_H
#define _UTILS_H

#include "main.h"

#include <string>

#define SIGN(x) ((x >= 0) ? 1 : -1)

#define MIN(x, y) ((x < y) ? (x) : (y))
#define MAX(x, y) ((x > y) ? (x) : (y))
#define CLAMP(lo, x, hi) ((x < lo) ? (lo) : ((x > hi) ? (hi) : (x)))

#define MAX3(x, y, z) ((x > y) ? ((x > z) ? x : z) : y)
#define MIN3(x, y, z) ((x < y) ? ((x < z) ? x : z) : y)

// return a random integer in the range [lo,hi] (inclusive)
inline int rand_int(int lo, int hi) {
  return lo +
         (int)((hi - lo + 1) * ((double)rand() / (double)(RAND_MAX + 1.0)));
}

// return a random real in the range [lo,hi] (inclusive)
inline double rand_real(double lo, double hi) {
  return lo + (hi - lo) * ((double)rand() / (double)(RAND_MAX + 1.0));
}

// conversion
string to_string(int val);
int to_int(string val);
double to_double(string val);

// capitalize all words, ie "hi there" -> "Hi There"
string capitalize(string str);

// return the data dir (where the sprites, sounds, etc are stored)
string get_data_dir();

#endif  // header guard
