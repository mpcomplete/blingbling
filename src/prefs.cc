#include "prefs.h"
#include "app.h"
#include "parser.h"
#include "utils.h"

Preferences prefs;

#define LOAD_INT(pref)              \
  if (cmd.get_command() == #pref) { \
    pref.val = cmd.get_int_arg(1);  \
    return 0;                       \
  }

#define SAVE_INT(pref) parser.write_line(#pref " " + to_string(pref.val))

#define SAVE_ARRAY(pref, start, n)                     \
  for (int i = start; i < n; i++) {                    \
    parser.write_line(#pref " " + to_string(i) + " " + \
                      to_string(pref[i].val));         \
  }

void Preferences::init() {
  num_types = PrefInt(3, NUM_COLORS, NUM_COLORS);
  block_size = PrefInt(2, 6, 2);
  field_width = PrefInt(4, 20, 6);
  field_height = PrefInt(4, 20, 12);
  font_size = PrefInt(1, 100, app->DefaultFont->GetSize());
  music_volume = PrefInt(0, MIX_MAX_VOLUME, 70);
  sfx_volume = PrefInt(0, MIX_MAX_VOLUME, 100);
  tile_size = PrefInt(0, 1 << 15, 50);

  key[KEY_DOWN] = PrefInt(0, 1 << 15, SDLK_DOWN);
  key[KEY_LEFT] = PrefInt(0, 1 << 15, SDLK_LEFT);
  key[KEY_RIGHT] = PrefInt(0, 1 << 15, SDLK_RIGHT);
  key[KEY_ROTATE_LEFT] = PrefInt(0, 1 << 15, SDLK_e);
  key[KEY_ROTATE_RIGHT] = PrefInt(0, 1 << 15, SDLK_r);
  key[KEY_DROP] = PrefInt(0, 1 << 15, SDLK_SPACE);

  Parser parser(PREFS_CONF);
  parser.set_handler("", this, &Preferences::cmd_pref);
  parser.set_handler("key", this, &Preferences::cmd_key);
  parser.parse_file();
}

void Preferences::save() {
  Parser parser(PREFS_CONF, PG_OPEN_WRITE);

  SAVE_INT(num_types);
  SAVE_INT(block_size);
  SAVE_INT(field_width);
  SAVE_INT(field_height);
  SAVE_INT(font_size);
  SAVE_INT(music_volume);
  SAVE_INT(sfx_volume);
  SAVE_INT(tile_size);
  SAVE_ARRAY(key, 0, NUM_KEYS);
}

void Preferences::set_defaults() {
  num_types.reset();
  block_size.reset();
  field_height.reset();
  field_width.reset();
  font_size.reset();
  music_volume.reset();
  sfx_volume.reset();
  tile_size.reset();
}

void Preferences::clamp() {
  num_types.clamp();
  block_size.clamp();
  field_height.clamp();
  field_width.clamp();
  font_size.clamp();
  music_volume.clamp();
  sfx_volume.clamp();
  tile_size.clamp();
}

// <prefname> <value>
int Preferences::cmd_pref(const ParserCmd& cmd) {
  if (cmd.num_args() != 1) {
    return Parser::ERR_SYNTAX;
  }

  LOAD_INT(num_types);
  LOAD_INT(block_size);
  LOAD_INT(field_width);
  LOAD_INT(field_height);
  LOAD_INT(font_size);
  LOAD_INT(music_volume);
  LOAD_INT(sfx_volume);
  LOAD_INT(tile_size);

  return Parser::ERR_BAD_COMMAND;
}

// key <index> <keysym value>
int Preferences::cmd_key(const ParserCmd& cmd) {
  if (cmd.num_args() != 2) {
    return Parser::ERR_SYNTAX;
  }

  int idx = cmd.get_int_arg(1);
  int sym = cmd.get_int_arg(2);
  key[idx].val = sym;

  return 0;
}
