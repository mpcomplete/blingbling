#ifndef _NEWGAMESCREEN_H
#define _NEWGAMESCREEN_H

#include "main.h"
#include "widgets.h"
#include "wscreen.h"

#include "pgbutton.h"
#include "pglabel.h"

class NewGameScreen : public WScreen {
 public:
  NewGameScreen();
  virtual ~NewGameScreen() {}

  virtual void resize();

  virtual bool eventKeyDown(const SDL_KeyboardEvent* key);

  /// update my prefs widgets based on the prefs settings
  void update();

  // whether or not we're dealing with a standard game
  void set_standard(bool val);

 protected:
  PG_Label ltitle;
  PG_Button bstart;
  WSpinPref snum_types;
  PG_Label lfun;
  PG_Button bstandards;
  WSpinPref sblock_size;
  WSpinPref sfield_width;
  WSpinPref sfield_height;
  PG_Label lnote1;
  PG_Label lnote2;
  PG_Button bmenu;
};

#endif  // header guard
