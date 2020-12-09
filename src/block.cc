#include "block.h"
#include "app.h"
#include "gamesingle.h"

#include <math.h>

// gotta make a closed loop
#define ANGLE_EAST (0)
#define ANGLE_NORTH (M_PI / 2)
#define ANGLE_WEST (M_PI)
#define ANGLE_SOUTH (3 * M_PI / 2)
#define ANGLE_EAST2 (2 * M_PI)

// how long to animate rotate, in milliseconds
#define ROTATION_DURATION (150.0)

void get_offsets(Block::Orientation orient, float* dx, float* dy) {
  switch (orient) {
    case Block::NORTH:
      *dx = 0.0f;
      *dy = 1.0f;
      break;
    case Block::EAST:
      *dx = 1.0f;
      *dy = 0.0f;
      break;
    case Block::SOUTH:
      *dx = 0.0f;
      *dy = -1.0f;
      break;
    case Block::WEST:
      *dx = -1.0f;
      *dy = 0.0f;
      break;
  }
}

float get_angle(Block::Orientation orient) {
  switch (orient) {
    case Block::EAST:
      return ANGLE_EAST;
      break;
    case Block::NORTH:
      return ANGLE_NORTH;
      break;
    case Block::WEST:
      return ANGLE_WEST;
      break;
    case Block::SOUTH:
      return ANGLE_SOUTH;
      break;
  }
  return 0.0f;
}

Block::Block() : orient(NORTH), y(0.0f), x(0.0f), dy(1.0f), dx(0.0f) {}

Block::Block(float y0, float x0)
    : orient(NORTH),
      tiles(prefs.block_size.val),
      y(y0),
      x(x0),
      dy(1.0f),
      dx(0.0f) {
  for (size_t i = 0; i < tiles.size(); i++) {
    tiles[i] = Tile();
  }
  this->start_tiles();
}

void Block::mimic(const Block& b) {
  orient = b.orient;
  x = b.x;
  y = b.y;
}

void Block::start_tiles() {
  for (size_t i = 0; i < tiles.size(); i++) {
    tiles[i].start();
  }
}

void Block::stop_tiles() {
  for (size_t i = 0; i < tiles.size(); i++) {
    tiles[i].stop();
  }
}

float Block::get_y(int n) const {
  return y + n * dy;
}

float Block::get_x(int n) const {
  return x + n * dx;
}

int Block::get_row(int n) const {
  if (orient == NORTH) {
    return int(floor(y + n));
  }
  if (orient == SOUTH) {
    return int(floor(y - n));
  }
  return int(floor(y));
}

int Block::get_col(int n) const {
  if (orient == EAST) {
    return int(floor(x + n));
  }
  if (orient == WEST) {
    return int(floor(x - n));
  }
  return int(floor(x));
}

void Block::tick(int ms) {
  if (!app->playing()) {
    // only animate if someone's watching
    return;
  }

  timer += ms;

  if (timer >= ROTATION_DURATION) {
    this->stop();
    get_offsets(orient, &dx, &dy);
    app->request_refresh();
    return;
  }

  double t = double(timer) / ROTATION_DURATION;
  double angle = (1.0 - t) * double(start_angle) + t * double(stop_angle);
  dy = sin(angle);
  dx = cos(angle);

  app->request_refresh();
}

// angle decreases
void Block::rotate_right(bool anim) {
  Orientation old_orient = orient;
  orient = Orientation((orient + 1) % 4);
  if (anim) {
    timer = 0;
    start_angle = get_angle(old_orient);
    stop_angle = get_angle(orient);
    if (stop_angle > start_angle) {
      start_angle += 2 * M_PI;
    }
    this->start();
  } else {
    get_offsets(orient, &dx, &dy);
  }
}

// angle increases
void Block::rotate_left(bool anim) {
  Orientation old_orient = orient;
  orient = Orientation((orient + 4 - 1) % 4);
  if (anim) {
    timer = 0;
    start_angle = get_angle(old_orient);
    stop_angle = get_angle(orient);
    if (stop_angle < start_angle) {
      stop_angle += 2 * M_PI;
    }
    this->start();
  } else {
    get_offsets(orient, &dx, &dy);
  }
}
