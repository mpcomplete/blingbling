#ifndef _SOUNDEFFECT_H
#define _SOUNDEFFECT_H

#include "main.h"
#include "parser.h"

#include "SDL_mixer.h"

#include <string>
#include <vector>

#define SOUND_CLEARED(n)                           \
  ((n - 1) < app->sfx_clear.num_sounds() ? (n - 1) \
                                         : app->sfx_clear.num_sounds() - 1)

#define SOUND_SPEEDUP 0
#define SOUND_TILEDUMP 1
#define SOUND_LOCK 2

class SoundSet {
 public:
  typedef int Type;
  typedef vector<Mix_Chunk*> Container;

  SoundSet();
  ~SoundSet();

  void play_sound(Type type);
  int cmd_sound_type(const ParserCmd& cmd, Type type);
  int cmd_sound_speedup(const ParserCmd& cmd);
  int cmd_sound_tiledump(const ParserCmd& cmd);
  int cmd_sound_lock(const ParserCmd& cmd);
  int cmd_sound_cleared(const ParserCmd& cmd);

  int num_sounds() { return sounds.size(); }

 private:
  Container sounds;
};

#endif  // header
