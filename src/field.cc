#include "field.h"
#include "app.h"
#include "gamesingle.h"
#include "utils.h"

Field::Field(int width0, int height0) : width(width0), height(height0) {
  current = NULL;

  data = new Tile*[height];
  connected = new bool*[height];
  for (int i = 0; i < height; i++) {
    data[i] = new Tile[width](TILE_EMPTY);
    connected[i] = new bool[width];
    for (int j = 0; j < width; j++)
      connected[i][j] = false;
  }

  next = new Block(height - 0.5, width / 2);
  set_state(BLOCK_RELEASE);
  score = 0;
  combo = 0;
  best_combo = 0;
  speed = 0;
  hazard_num = 0;
  hazard_timer = 0;
  timer = 0;

  field_changed = true;
}

Field::~Field() {
  for (int i = 0; i < height; i++)
    delete[] data[i];
  delete[] data;
}

void Field::tick(int ms) {
  timer += ms;

  if (state != WIPE_CLEARED) {
    hazard_timer += ms;
  }

  switch (state) {
    case BLOCK_RELEASE:
      block_release();
      timer = 0;
      break;
    case BLOCK_FALLING:
      if (timer >= (325 - speed * 13)) {
        block_move_down();
        timer = 0;
      }
      break;
    case FORM_CONNECTIONS:
      if (timer >= 400) {
        form_connections();
        timer = 0;
      }
      break;
    case CLEAR_SETS:
      if (timer >= 400) {
        clear_sets();
        timer = 0;
      }
      break;
    case WIPE_CLEARED: {
      int delay_wipe = app->sprites.get_sprite(TILE_CLEARED)->total_duration();
      if (timer >= delay_wipe) {
        wipe_cleared();
        timer = 0;
      }
      break;
    }
    case HAZARD_CHECK:
      hazard_check();
      break;
    case GAME_OVER:
      break;
  }
}

int Field::check_collision(const Block& block) {
  int num_collide = 0;
  for (int i = 0; i < prefs.block_size.val; i++) {
    int row = block.get_row(i);
    int col = block.get_col(i);

    // special case: off the top doesn't count as a collision
    if (row >= height && ON_FIELD(0, col))
      continue;

    if (!ON_FIELD(row, col) || data[row][col].type != TILE_EMPTY)
      num_collide++;
  }
  return num_collide;
}

void Field::block_release() {
  if (check_collision(*next)) {
    set_state(GAME_OVER);
    return;
  }

  current = next;
  next = new Block(height - 0.5, width / 2);

  app->request_refresh();
  set_state(BLOCK_FALLING);
}

bool Field::block_move_down() {
  if (!IS_BLOCK_FALLING())
    return false;

  app->request_refresh();

  Block b;
  b.mimic(*current);
  b.move_down(0.5);

  if (!check_collision(b)) {
    current->mimic(b);
    return true;
  }
  block_lock();
  return false;
}

bool Field::block_move_left() {
  if (!IS_BLOCK_FALLING())
    return false;

  app->request_refresh();

  Block b;
  b.mimic(*current);
  b.move_left();

  if (!check_collision(b)) {
    current->mimic(b);
    return true;
  }
  return false;
}

bool Field::block_move_right() {
  if (!IS_BLOCK_FALLING())
    return false;

  app->request_refresh();

  Block b;
  b.mimic(*current);
  b.move_right();

  if (!check_collision(b)) {
    current->mimic(b);
    return true;
  }
  return false;
}

// left = counterclockwise
bool Field::block_rotate_left() {
  if (!IS_BLOCK_FALLING())
    return false;

  app->request_refresh();

  int n;
  Block b;
  b.mimic(*current);
  b.rotate_left(false);

  if ((n = check_collision(b)) == 0) {
    current->rotate_left(true);
    return true;
  }

  // if we can't rotate, see if we can do a move+rotate.
  // note: we have to move it by the number of blocks that have collided
  switch (current->get_orient()) {
    case Block::NORTH:
      b.move_right(float(n));
      break;
    case Block::SOUTH:
      b.move_left(float(n));
      break;
    default:
      return false;
  }
  if (!check_collision(b)) {
    // kinda hackish
    b.rotate_right(false);
    current->mimic(b);
    current->rotate_left(true);
    return true;
  }
  return false;
}

// right = clockwise
bool Field::block_rotate_right() {
  if (!IS_BLOCK_FALLING())
    return false;

  app->request_refresh();

  int n;
  Block b;
  b.mimic(*current);
  b.rotate_right(false);

  if ((n = check_collision(b)) == 0) {
    current->rotate_right(true);
    return true;
  }

  // if we can't rotate, see if we can do a move+rotate.
  // note: we have to move it by the number of blocks that have collided
  switch (current->get_orient()) {
    case Block::NORTH:
      b.move_left(float(n));
      break;
    case Block::SOUTH:
      b.move_right(float(n));
      break;
    default:
      return false;
  }
  if (!check_collision(b)) {
    // kinda hackish
    b.rotate_left(false);
    current->mimic(b);
    current->rotate_right(true);
    return true;
  }
  return false;
}

bool Field::block_lock() {
  if (!IS_BLOCK_FALLING())
    return false;

  app->request_refresh();
  set_field_changed(true);

  current->stop_tiles();

  for (int i = 0; i < prefs.block_size.val; i++) {
    // go from lowest to highest Tile
    int n = i;
    if (current->get_orient() == Block::SOUTH)
      n = prefs.block_size.val - 1 - i;

    int row = current->get_row(n);
    int col = current->get_col(n);

    while ((row > 0) &&
           (row >= height || data[row - 1][col].type == TILE_EMPTY))
      row--;

    if (row >= height || data[row][col].type != TILE_EMPTY) {
      set_state(GAME_OVER);
      return false;
    }

    assert(ON_FIELD(row, col));
    data[row][col] = current->get_tile(n);
  }

  delete current;
  current = NULL;

  set_state(FORM_CONNECTIONS);

  return true;
}

void Field::form_connections() {
  bool new_connection = false;

  app->request_refresh();
  set_field_changed(true);

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      const Tile& tile = this->get_tile(row, col);
      if (IS_COLOR(tile.type) &&
          ((col + 1 < width &&
            tile.type == this->get_tile(row, col + 1).type) ||
           (row + 1 < height &&
            tile.type == this->get_tile(row + 1, col).type) ||
           (col - 1 >= 0 && tile.type == this->get_tile(row, col - 1).type) ||
           (row - 1 >= 0 && tile.type == this->get_tile(row - 1, col).type))) {
        if (!connected[row][col]) {
          new_connection = true;
          connected[row][col] = true;
        }
      } else {
        connected[row][col] = false;
      }
    }
  }

  if (new_connection) {
    app->sfx.play_sound(SOUND_LOCK);
  }

  set_state(CLEAR_SETS);
}

int Field::add_score(int num_cleared) {
  if (num_cleared == 0) {
    // give them a point if they didn't get a clear
    // come to think of it, NO! I'm going to be stingy with my points.
    // screw you all.
    //	if (combo == 0)
    //	    score += 1;

    if (combo > best_combo)
      best_combo = combo;
    combo = 0;

    set_state(HAZARD_CHECK);
    return 0;
  } else {
    combo++;

    /*
    scoring: (assuming 4 is the CLEAR_MIN)
        multiplier bonus for more than 4 sprites in a clear
        multiplier bonus for combos
        P = pts for a basic (4 tile) clear
        T = number of sprites cleared
        C = combo number (1 for first clear, 2 for clear-fall-clear, etc)
        tile bonus = 1 + (T-4)/2   (+50% for each extra tile)
        combo bonus = 3^(C-1)
        score = tile * combo * P
                           4tiles  5tiles  6tiles
        combo1 =  1 * 4 =	4	6	8
        combo2 =  3 * 4 =	12	18	24
        combo3 =  9 * 4 =	28	54	56
        combo4 = 27 * 4 =	108	162	216
        combo5 = 81 * 4 =	324	486	648
    */
    double tbonus = 1 + double(num_cleared - CLEAR_MIN) / 2.0;
    double cbonus = pow(3.0, combo - 1.0);
    double bonus = tbonus * cbonus;
    int gain = (int)(bonus * POINTS_PER_CLEAR);
    score += gain;

    app->sfx_clear.play_sound(SOUND_CLEARED(combo));
    hazard_decrease((int)(bonus * HAZARD_DROP_PER_CLEAR));

    set_state(WIPE_CLEARED);
    return gain;
  }
}

void Field::clear_sets() {
  app->request_refresh();
  set_field_changed(true);

  int num_cleared = 0;

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (!IS_COLOR(data[row][col].type))
        continue;
      if (check_set_from(row, col, data[row][col].type) >= CLEAR_MIN)
        num_cleared += clear_set_from(row, col, data[row][col].type);
    }
  }

  add_score(num_cleared);
}

int Field::check_set_from(int row, int col, Tile::Type type) {
  if (!ON_FIELD(row, col) || data[row][col].type != type)
    return 0;

  Tile tmp = data[row][col];
  data[row][col] = Tile(TILE_CLEARED);

  int num = 1;
  num += check_set_from(row - 1, col, type);
  num += check_set_from(row + 1, col, type);
  num += check_set_from(row, col - 1, type);
  num += check_set_from(row, col + 1, type);

  data[row][col] = tmp;

  return num;
}

int Field::clear_set_from(int row, int col, Tile::Type type) {
  if (!ON_FIELD(row, col) || data[row][col].type != type)
    return 0;

  connected[row][col] = false;
  data[row][col] = Tile(TILE_CLEARED);
  data[row][col].start();

  int num = 1;
  num += clear_set_from(row - 1, col, type);
  num += clear_set_from(row + 1, col, type);
  num += clear_set_from(row, col - 1, type);
  num += clear_set_from(row, col + 1, type);

  return num;
}

void Field::wipe_cleared() {
  app->request_refresh();
  set_field_changed(true);

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (IS_COLOR(data[row][col].type))
        continue;

      data[row][col] = Tile(TILE_EMPTY);
      int tmp = row;
      for (int above = row; above < height; above++) {
        if (!IS_COLOR(data[above][col].type)) {
          data[above][col] = Tile(TILE_EMPTY);
          continue;
        }
        connected[tmp][col] = connected[above][col];
        connected[above][col] = false;
        data[tmp][col] = data[above][col];
        data[above][col] = Tile(TILE_EMPTY);

        tmp++;
      }
    }
  }
  set_state(FORM_CONNECTIONS);
}

void Field::hazard_check() {
  if (hazard_timer >= 1000 * HAZARD_INTERVAL) {
    hazard_timer = 0;
    hazard_num++;
    if ((hazard_num % 2) == 1) {
      app->sfx.play_sound(SOUND_SPEEDUP);
      speed++;
    } else {
      app->sfx.play_sound(SOUND_TILEDUMP);
      this->hazard_tiledump();
    }
  }

  set_state(BLOCK_RELEASE);
}

void Field::hazard_tiledump() {
  app->request_refresh();

  // first check if this hazard will push blocks off the top
  for (int col = 0; col < width; col++) {
    if (IS_COLOR(data[height - 1][col].type)) {
      set_state(GAME_OVER);
      return;
    }
  }

  // move everything up 1
  for (int row = height - 1; row > 0; row--) {
    for (int col = 0; col < width; col++) {
      data[row][col] = data[row - 1][col];
    }
  }

  // add the hazard row
  for (int col = 0; col < width; col++) {
    // make sure we don't accidentally get any clears
    // just don't try TOO hard
    for (int time = 0; time < 50; time++) {
      data[0][col] = Tile();
      // make sure it's not the same as the one above
      if (data[0][col].type == data[1][col].type) {
        continue;
      }
      // prevent 3-in-a-rows
      if (col >= 2 && data[0][col - 2].type == data[0][col - 1].type &&
          data[0][col - 1].type == data[0][col].type) {
        continue;
      }
      break;
    }
  }
  form_connections();
}

void Field::hazard_decrease(int percent) {
  hazard_timer -= percent * 10 * HAZARD_INTERVAL;
  if (hazard_timer < 0)
    hazard_timer = 0;
}
