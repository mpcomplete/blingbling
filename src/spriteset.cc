#include "spriteset.h"
#include "app.h"
#include "parser.h"
#include "prefs.h"
#include "utils.h"

#include "SDL_image.h"
#include "pgdraw.h"
#include "pgrect.h"

SDL_Surface* load_image(string file, int w, int h) {
  SDL_RWops* rwops = app->OpenFileRWops(file.c_str());
  SDL_Surface* tmp = IMG_Load_RW(rwops, true);
  SDL_Surface* image;

  if (!tmp)
    return 0;

  if (tmp->w == w && tmp->h == h) {
    image = SDL_DisplayFormatAlpha(tmp);
  } else {
    double zoomx = double(w) / double(tmp->w);
    double zoomy = double(h) / double(tmp->h);
    SDL_Surface* tmp2 = PG_Draw::ScaleSurface(tmp, zoomx, zoomy, true);
    image = SDL_DisplayFormatAlpha(tmp2);
    SDL_FreeSurface(tmp2);
  }

  SDL_FreeSurface(tmp);

  return image;
}

void Sprite::add_frame(int duration, string filename, int width, int height) {
  Frame f;

  f.filename = filename;
  f.duration = duration;
  f.surface = load_image(filename, width, height);
  if (!f.surface) {
    PG_LogWRN("cannot load sprite: %s", filename.c_str());
  }
  frames.push_back(f);
}

void Sprite::reload(int width, int height) {
  for (size_t i = 0; i < frames.size(); i++) {
    if (frames[i].surface) {
      SDL_FreeSurface(frames[i].surface);
      frames[i].surface = NULL;
    }
    frames[i].surface = load_image(frames[i].filename, width, height);
    if (!frames[i].surface) {
      PG_LogWRN("cannot load sprite: %s", frames[i].filename.c_str());
    }
  }
}

void Sprite::free() {
  for (size_t i = 0; i < frames.size(); i++) {
    if (frames[i].surface) {
      SDL_FreeSurface(frames[i].surface);
      frames[i].surface = NULL;
    }
  }
}

unsigned Sprite::total_duration() {
  unsigned total = 0;
  for (size_t i = 0; i < frames.size(); i++) {
    total += frames[i].duration;
  }
  return total;
}

SpriteSet::~SpriteSet() {
  for (size_t i = 0; i < sprites.size(); i++) {
    sprites[i].free();
  }
}

void SpriteSet::reload(int width, int height) {
  for (size_t i = 0; i < sprites.size(); i++) {
    sprites[i].reload(width, height);
  }
}

void SpriteSet::blit_tile(Tile::Type type,
                          unsigned frame,
                          SDL_Surface* dest,
                          int x,
                          int y) {
  Sprite* sprite;
  SDL_Surface* surf;
  if ((sprite = get_sprite(type)) && (surf = sprite->get_surface(frame))) {
    SDL_BlitSurface(surf, NULL, dest, PG_Rect(x, y, 0, 0).SDLRect());
  }
}

Sprite* SpriteSet::get_sprite(Tile::Type type) {
  if (type < 0 || type >= (int)sprites.size()) {
    return NULL;
  }

  return &sprites[type];
}

int SpriteSet::cmd_tile(const ParserCmd& cmd) {
  if (cmd.num_args() < 2) {
    return Parser::ERR_SYNTAX;
  }
  Sprite s;

  for (int i = 1; i + 1 <= cmd.num_args(); i += 2) {
    s.add_frame(cmd.get_int_arg(i), cmd.get_str_arg(i + 1), width, height);
  }
  sprites.push_back(s);
  return 0;
}
