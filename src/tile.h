// a single "tile" - one tile of a block. is associated with a spriteset.

#ifndef _TILE_H
#define _TILE_H

#include "main.h"
#include "ticker.h"

#define TILE_EMPTY -1
#define TILE_CLEARED 0
#define TILE_START 1
#define NUM_COLORS (app->sprites.num_tiles() - 1)

#define IS_COLOR(color) \
  (TILE_START <= (color) && (color) <= prefs.num_types.val)

class Tile : public Ticker {
 public:
  typedef int Type;

  Type type;
  unsigned age;
  unsigned frame;

 public:
  // new Tile with random nonEMPTY type
  Tile();
  // new Tile with specified type
  Tile(Type type0);
  void tick(int ms);
};

Tile::Type rand_tiletype();

#endif  // header guard
