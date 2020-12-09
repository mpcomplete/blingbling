#ifndef _HELPSCREEN_H
#define _HELPSCREEN_H

#include "main.h"
#include "parser.h"
#include "spriteset.h"
#include "wscreen.h"

#include "pgbutton.h"

class HelpScreen : public WScreen {
  SpriteSet helppages;
  int curpage;

  PG_Button bprev;
  PG_Button bnext;
  PG_Button bmenu;

 public:
  HelpScreen();
  virtual ~HelpScreen() {}

  int get_curpage() { return curpage; }
  // flip to page #.  Starts at 0.
  void show_page(int n);

  virtual void resize();

  virtual void eventBlit(SDL_Surface* surf,
                         const PG_Rect& src,
                         const PG_Rect& dst);
  virtual bool eventKeyDown(const SDL_KeyboardEvent* key);
};

#endif  // header guard
