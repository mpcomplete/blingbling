// the "block" - a set of 2 (most likely) tiles that move as 1 block

#ifndef _BLOCK_H
#define _BLOCK_H

#include "prefs.h"
#include "ticker.h"
#include "tile.h"

#include <vector>

// each Block is made up of a number of tiles, and an orientation.
class Block : public Ticker {
 public:
  // orientation is defined like a compass (with 0 as the center tile)
  enum Orientation {
    NORTH = 0,  //  1
                //  0

    EAST = 1,  //  01

    SOUTH = 2,  //  0
                //  1

    WEST = 3  // 10
  };

 public:
  // new block with no defaults
  Block();
  // new block with x,y as the center tile's position
  Block(float y, float x);

  Orientation get_orient() { return orient; }
  const Tile& get_tile(int i) const {
    assert(i >= 0 && i < prefs.block_size.val);
    return tiles[i];
  }

  /**
   * go to the same position/orientation as block b
   */
  void mimic(const Block& b);

  void start_tiles();  ///< start the tiles ticking
  void stop_tiles();   ///< stop the tiles ticking

  // get the ith tile's on-screen x or y position
  float get_y(int n) const;
  float get_x(int n) const;

  // get the ith tile's row or col (not necessarily same as get_y)
  int get_row(int n) const;
  int get_col(int n) const;

  // move the block
  void move_up(float n = 1.0) { y += n; }
  void move_down(float n = 1.0) { y -= n; }
  void move_left(float n = 1.0) { x -= n; }
  void move_right(float n = 1.0) { x += n; }

  // update the rotation
  void tick(int ms);
  // rotate right (clockwise), with animation if anim
  void rotate_right(bool anim = false);
  // rotate left (counterclockwise), with animation if anim
  void rotate_left(bool anim = false);

 private:
  Orientation orient;  ///< how the block is oriented (rotated)
  vector<Tile> tiles;  ///< the N tiles of the block
  float y;             ///< the y position (row)
  float x;             ///< the x position (column)
  float dy;            ///< the y distance from the center for each tile
  float dx;            ///< the x distance from the center for each tile
  int timer;           ///< rotation animation timer
  float start_angle;   ///< rotation angle to start at, in radians
  float stop_angle;    ///< rotation angle to end at, in radians
};

#endif  // header guard
