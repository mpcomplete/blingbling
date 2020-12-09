#ifndef _GAMESINGLE_H
#define _GAMESINGLE_H

#include "main.h"

#include "wfield.h"
#include "widgets.h"
#include "wscreen.h"

#include "pgbutton.h"
#include "pglabel.h"
#include "pgprogressbar.h"

#include <vector>

// an object of this type is created when a single player game is started
class GameSingle : public WScreen {
 protected:
  bool paused;

  int last_speed;
  int last_combo;
  int last_score;

  // holds a nonzero time indicating when to handle a keypress if the key
  // is currently pressed.
  int key_delay[NUM_KEYS];
  bool key_handled[NUM_KEYS];

  // widgets
  WField wfield;
  WBlock wnextblock;
  PG_Label lscore;
  PG_Label lspeed;
  PG_Label lcombo;
  PG_Label ldifficulty;
  PG_ProgressBar whazard;
  PG_Button bmenu;

  WScoreWin* scorewin;

 public:
  GameSingle();
  virtual ~GameSingle();

  WField* field() { return &wfield; }

  void floatscore_new(int gain);

  virtual void resize();
  virtual void tick(int ms);
  virtual bool eventKeyDown(const SDL_KeyboardEvent* key);
  virtual bool eventKeyUp(const SDL_KeyboardEvent* key);
};

#endif  // header
