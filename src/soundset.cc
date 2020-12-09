#include "soundset.h"
#include "app.h"
#include "parser.h"
#include "prefs.h"
#include "utils.h"

SoundSet::SoundSet() {}

SoundSet::~SoundSet() {
  for (size_t i = 0; i < sounds.size(); i++) {
    if (sounds[i])
      Mix_FreeChunk(sounds[i]);
  }
}

void SoundSet::play_sound(Type type) {
  if (!app->has_audio())
    return;

  if (type < 0 || type >= (Type)sounds.size() || !sounds[type]) {
    return;
  }

  Mix_VolumeChunk(sounds[type], prefs.sfx_volume.val);
  Mix_PlayChannel(-1, sounds[type], 0);
}

int SoundSet::cmd_sound_type(const ParserCmd& cmd, Type type) {
  string file = cmd.get_str_arg(1);
  SDL_RWops* rwops = app->OpenFileRWops(file.c_str());
  Mix_Chunk* chunk = Mix_LoadWAV_RW(rwops, true);
  if (sounds.size() < size_t(type + 1)) {
    sounds.resize(type + 1);
  }
  sounds[type] = chunk;

  return 0;
}

int SoundSet::cmd_sound_speedup(const ParserCmd& cmd) {
  return cmd_sound_type(cmd, SOUND_SPEEDUP);
}

int SoundSet::cmd_sound_tiledump(const ParserCmd& cmd) {
  return cmd_sound_type(cmd, SOUND_TILEDUMP);
}

int SoundSet::cmd_sound_lock(const ParserCmd& cmd) {
  return cmd_sound_type(cmd, SOUND_LOCK);
}

int SoundSet::cmd_sound_cleared(const ParserCmd& cmd) {
  string file = cmd.get_str_arg(1);
  SDL_RWops* rwops = app->OpenFileRWops(file.c_str());
  Mix_Chunk* chunk = Mix_LoadWAV_RW(rwops, true);
  sounds.push_back(chunk);

  return 0;
}
