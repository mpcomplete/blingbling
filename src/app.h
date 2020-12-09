#ifndef _APP_H
#define _APP_H

#include "main.h"
#include "parser.h"
#include "scorelist.h"
#include "soundset.h"
#include "spriteset.h"

#include "SDL_mixer.h"
#include "pgapplication.h"

class WScreen;
class GameSingle;
class MenuScreen;
class OptionScreen;

class App : public PG_Application {
 protected:
  bool m_need_refresh;
  bool audio_open;
  Mix_Music* music;

  BasicCallback* idlefunc;
  void* idledata;

  WScreen* wscreen;
  GameSingle* m_game;

 public:
  ScoreList scores;
  SpriteSet sprites;
  SpriteSet sprites_connect;
  SoundSet sfx;
  SoundSet sfx_clear;

  App();
  virtual ~App() {}

  // let the game begin!
  // this is our main function. it initializes audio and video, and runs
  // the event loop. it returns when the game has been quit.
  int run();

  bool need_refresh() { return m_need_refresh; }
  void request_refresh(bool val = true) { m_need_refresh = val; }

  bool has_audio() { return audio_open; }
  void update_music();

  // set this function to be executed at next idle loop
  // FIXME: use a timer
  void set_idle_func(BasicCallback* func, void* data) {
    idlefunc = func;
    idledata = data;
  }

  WScreen* screen() { return wscreen; }
  GameSingle* game() { return m_game; }
  bool playing() { return (m_game && m_game == (GameSingle*)wscreen); }

  int get_font_size();

  // change screens.  The current one is deleted, unless it is a game.
  // if mnew is NULL, make the mainmenu the new screen, otherwise use mnew.
  void change_screen(WScreen* mnew = NULL);
  // change screens, but also delete the previous game.
  void change_screen(GameSingle* m);
  // ensure that the currect screen is deleted.
  void close_screen();

  void Quit();

  static int get_ms() { return SDL_GetTicks(); }

 protected:
  // initialize the SDL screen
  int init_video();
  // open audio and start playing bg music
  int init_audio();

  // resize the SDL window to width*height
  bool resize(int width, int height);

  void tick(int ms);
  virtual void eventIdle() { tick(elapsed_ms()); }
  virtual bool eventResize(const SDL_ResizeEvent* event);

  // return number of milliseconds since this was last called
  static int elapsed_ms();

 private:
  // load the blingbling.conf file
  void load_config();
  int cmd_music(const ParserCmd& cmd);
};

#endif  // header
