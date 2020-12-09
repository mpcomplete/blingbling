#ifndef _PREFS_H
#define _PREFS_H

#include "main.h"

#include "parser.h"

template <class T>
struct PrefType {
  T min, max;  // min and max
  T def;       // default
  T val;

  PrefType() {}
  PrefType(T imin, T imax, T idef)
      : min(imin), max(imax), def(idef), val(idef) {}
  inline bool is_default() { return (val == def); }
  inline void reset() { val = def; }
  inline void clamp() {
    if (val < min)
      val = min;
    else if (val > max)
      val = max;
  }
};

typedef PrefType<int> PrefInt;

struct Preferences {
  PrefInt num_types;     // number of types/colors
  PrefInt block_size;    // sprites per block
  PrefInt field_height;  // game field height
  PrefInt field_width;   // game field width
  PrefInt font_size;     // application font size
  PrefInt music_volume;  // music volume
  PrefInt sfx_volume;    // sound effect volume
  PrefInt tile_size;     // tile width and height (they must be square)

  PrefInt key[NUM_KEYS];  // the key bindings

  void init();
  void save();
  void set_defaults();
  void clamp();

 private:
  int cmd_pref(const ParserCmd& cmd);
  int cmd_key(const ParserCmd& cmd);
};

extern Preferences prefs;

#endif  // header guard
