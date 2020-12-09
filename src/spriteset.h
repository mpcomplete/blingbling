// image representation of our sprites

#ifndef _TILESET_H
#define _TILESET_H

#include "main.h"
#include "parser.h"
#include "tile.h"

#include <vector>
#include "SDL.h"

// Sprite does not destroy it's surface when it is destructed.
class Sprite {
 public:
  struct Frame {
    string filename;
    SDL_Surface* surface;
    unsigned duration;
  };
  vector<Frame> frames;

  Sprite() {}
  void add_frame(int duration, string filename, int width, int height);
  void reload(int width, int height);
  void free();
  SDL_Surface* get_surface(unsigned f) {
    return (f < frames.size() ? frames[f].surface : NULL);
  }
  unsigned get_duration(unsigned f) {
    return (f < frames.size() ? frames[f].duration : 0);
  }
  unsigned num_frames() { return frames.size(); }
  unsigned total_duration();
};

class SpriteSet {
 public:
  typedef vector<Sprite> Container;

  Container sprites;
  int width;
  int height;

  ~SpriteSet();

  void set_size(int w, int h) {
    width = w;
    height = h;
  }
  int num_tiles() { return sprites.size(); }

  // relead all sprites
  void reload(int width, int height);

  // blit the frame'th frame of the tile to the surface
  void blit_tile(Tile::Type type,
                 unsigned frame,
                 SDL_Surface* dest,
                 int x,
                 int y);

  // return the sprite corresponding to 'type'
  Sprite* get_sprite(Tile::Type type);

  int cmd_tile(const ParserCmd& cmd);
};

#endif  // header
