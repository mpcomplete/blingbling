#ifndef _OPTIONSCREEN_H
#define _OPTIONSCREEN_H

#include "main.h"
#include "prefs.h"
#include "widgets.h"
#include "wscreen.h"

#include "pgbutton.h"
#include "pglabel.h"

class OptionScreen : public WScreen {
 public:
  PG_Button* buttons[NUM_KEYS];
  PG_Label* labels[NUM_KEYS];
  int key_wait;

  OptionScreen();
  virtual ~OptionScreen() {}

  virtual void resize();

  virtual bool eventKeyDown(const SDL_KeyboardEvent* key);

 protected:
  PG_Label ltitle;
  WSlidePref smusic_volume;
  WSlidePref ssfx_volume;
  PG_Label lnote;
  PG_Button bdefaults;
  PG_Button bmenu;
};

#endif  // header guard
