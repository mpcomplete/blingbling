#include "tile.h"
#include "app.h"
#include "gamesingle.h"
#include "prefs.h"
#include "spriteset.h"
#include "utils.h"

#include <math.h>

Tile::Type rand_tiletype() {
  return rand_int(TILE_START, prefs.num_types.val);
}

Tile::Tile() : type(rand_tiletype()), age(0), frame(0) {}

Tile::Tile(Type type0) : type(type0), age(0), frame(0) {}

void Tile::tick(int ms) {
  Sprite* sprite = app->sprites.get_sprite(type);
  if (!sprite || sprite->num_frames() < 2) {
    frame = 0;
    this->stop();
    return;
  }
  if (!app->playing()) {
    // only tick if they're watching
    return;
  }

  age += ms;
  if (age >= sprite->get_duration(frame)) {
    age -= sprite->get_duration(frame);
    frame++;
    if (frame > sprite->num_frames()) {
      frame = 0;
    }
    app->request_refresh();
    app->game()->field()->set_field_changed(true);
  }
}
