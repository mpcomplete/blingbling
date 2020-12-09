// the playing field: where the pieces fall and stack up.

#ifndef _FIELD_H
#define _FIELD_H

#include "main.h"

#include "block.h"
#include "tile.h"

#define ON_FIELD(row, col) \
  ((row >= 0 && row < height) && (col >= 0 && col < width))

/// minimum number of sprites in a clear
#define CLEAR_MIN 4
// number of points per tile cleared
#define POINTS_PER_CLEAR (1 * CLEAR_MIN)

// how much (%) the hazard bar drops per clear
#define HAZARD_DROP_PER_CLEAR (5)
// hazard every x seconds
#define HAZARD_INTERVAL 20

#define IS_BLOCK_FALLING() (state == BLOCK_FALLING)

// The field is a simple 2d array of sprites. When a block is set into
// place, the next block is released, and a new next block is created.
// Bottom left is 0,0; top right is width-1,height-1.
class Field {
 public:
  Field(int width0 = 6, int height0 = 12);
  virtual ~Field();

  bool game_over() { return state == GAME_OVER; }

  int get_width() { return width; }
  int get_height() { return height; }

  int get_score() { return score; }
  int get_best_combo() { return best_combo; }
  int get_speed() { return speed; }
  double get_hazard_percent() {
    return double(hazard_timer) / double(HAZARD_INTERVAL * 10);
  }

  // returns the next block to fall
  Block* get_next() { return next; }
  // returns the currently falling block
  Block* get_block() { return current; }

  // returns the tile at row,col
  const Tile& get_tile(int row, int col) {
    assert(ON_FIELD(row, col));
    return data[row][col];
  }
  // returns whether the tile at row,col is connected to a nearby tile
  bool is_connected(int row, int col) {
    assert(ON_FIELD(row, col));
    return connected[row][col];
  }

  // advance one tick, depending on state:
  // if a block is falling, drop it down 1, or lock it if it hits another
  //  tile.
  // if we just locked a block, release a new one
  void tick(int ms);

  // check if this block is colliding with placed sprites/walls/floor
  int check_collision(const Block& block);

  // release the next block
  void block_release();

  // move or rotate the block, if possible.
  // return true if the block has been moved/rotated, false if unable.
  bool block_move_down();
  bool block_move_left();
  bool block_move_right();

  bool block_rotate_left();
  bool block_rotate_right();

  // drop the block all the way down and lock it in place.  if the
  // surface below is uneven, the block is severed and the sprites drop as
  // far as possible, independent of each other.
  // return true if block was locked, false if unable
  bool block_lock();

  // calculate which blocks are connected (stored in the connections[][]
  // matrix).
  void form_connections();

  // updates score and combo number after num_cleared pieces have been
  // cleared
  virtual int add_score(int num_cleared);

  // checks for sets of 4+ adjacent same-colors, and removes them
  // if there.  returns number of sets removed.
  void clear_sets();

  // checks if the tile at row,col is part of a set. returns number
  // of adjacent like-sprites.
  int check_set_from(int row, int col, Tile::Type type);

  // clears a set of adjacent sprites.  this should only be called after
  // check_set_from() returns the required amount (SET_MIN).  returns
  // number of sprites cleared.
  int clear_set_from(int row, int col, Tile::Type type);

  // remove cleared pieces from the field and drop everything down.
  void wipe_cleared();

  // check if we should do a hazard, and do it.
  void hazard_check();
  // do a tiledump hazard (an extra row of random tiles at the bottom)
  void hazard_tiledump();

  // decrease the hazard timer by percent
  void hazard_decrease(int percent);

  bool has_field_changed() { return field_changed; }
  void set_field_changed(bool val) { field_changed = val; }

 protected:
  enum State {
    BLOCK_RELEASE,     // a block is releasing
    BLOCK_FALLING,     // a block is falling
    FORM_CONNECTIONS,  // the block has fallen, now connect it to its
                       // surroundings
    CLEAR_SETS,        // clear sets of blocks
    WIPE_CLEARED,      // get rid of cleared sets
    HAZARD_CHECK,      // make a hazard after clears (if it's time)
    GAME_OVER
  };
  void set_state(State s) { state = s; }

  int width;         // width in sprites (def=6)
  int height;        // height in sprites (def=12)
  Tile** data;       // a height*width matrix of sprites
  bool** connected;  // a height*width matrix of connections
  Block* current;    // the current block out on the board
  Block* next;       // the next block to be released
  State state;       // is a block falling, game over, etc
  int timer;         // state timer
  int speed;         // how fast blocks drop
  int hazard_num;    // how many hazards have happened
  int hazard_timer;  // hazard (speedup, tiledump) timer
  int score;         // the score
  int combo;         // which combo: a combo occurs when a clear causes
                     // pieces to drop into a pattern that triggers another
                     // clear.
  int best_combo;    // best combo so far (starts at 1)

  bool field_changed;  // has the field changed since last redraw.
};

#endif  // _FIELD_H
