#ifndef _MAINMENU_H
#define _MAINMENU_H

#include "main.h"
#include "wscreen.h"

#include "pgbutton.h"
#include "pglabel.h"

class MenuScreen : public WScreen {
 protected:
  PG_Label ltitle;
  PG_Button bresumegame;
  PG_Button bsinglegame;
  PG_Button boptions;
  PG_Button bhighscores;
  PG_Button bhelp;
  PG_Button bcredits;
  PG_Button bexit;

 public:
  MenuScreen();
  virtual ~MenuScreen() {}

  virtual void resize();

  virtual bool eventKeyDown(const SDL_KeyboardEvent* key);
};

#endif  // header guard
